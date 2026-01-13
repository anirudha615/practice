#include <iostream>
#include <mutex>
#include <shared_mutex>

#include <map>

using namespace std;

struct ValueRecord {
    string m_value;
    bool m_withTimestampQuery = true;
    int64_t m_timestamp = 0;
    int64_t m_expiryTimestamp = 0;

    ValueRecord(const std::string& value): m_value(value), m_withTimestampQuery(false) {}

    ValueRecord(const std::string& value, const int64_t& timestamp, int64_t expiryTimestamp)
    : m_value(value), m_timestamp(timestamp), m_expiryTimestamp(expiryTimestamp) {}

    string getValue(int64_t queriedTimestamp) {
        if (!m_withTimestampQuery || (queriedTimestamp >= m_timestamp && queriedTimestamp < m_expiryTimestamp)) {
            return m_value;
        }
        return "";
    }
};

class FieldRecord {
private:
    std::map<string, std::shared_ptr<ValueRecord>> m_fieldValue;
    std::mutex m_mtx;
public:
    FieldRecord() {}

    void setField(const string& field, const string&  value) { // V1
        m_fieldValue[field] = std::make_shared<ValueRecord>(value);
    }

    void setField(const string& field, const string&  value, const int64_t& timestamp, const int64_t& expiryTimestamp) { // V2
        m_fieldValue[field] = std::make_shared<ValueRecord>(value, timestamp, expiryTimestamp);
    }

    string getField(const string& field, const int64_t& queriedTimestamp) { // V2
        if (!m_fieldValue.count(field)) {
            return "";
        }
        return m_fieldValue.at(field)->getValue(queriedTimestamp);
    }

    bool deleteField(const string& field, const int64_t& queriedTimestamp) { // V2
        if (!m_fieldValue.count(field)) {
            return false;
        }
        bool isExpired = m_fieldValue.at(field)->getValue(queriedTimestamp).size() == 0;
        m_fieldValue.erase(field);
        return !isExpired;
    }

    std::mutex& getMutex() {
        return m_mtx;
    }

    std::map<string, std::shared_ptr<ValueRecord>> getFieldValueMap() {
        return m_fieldValue;
    }
    

    int64_t getSize() {
        return m_fieldValue.size();
    }
};

class DatabaseManager {
private:
    std::shared_timed_mutex m_sharedStateMtx;
    std::unordered_map<string, std::shared_ptr<FieldRecord>> m_cache;

    void setDatabaseRecord(const string& key, const string& field, const string& value) { // V1
        std::shared_ptr<FieldRecord> record = std::make_shared<FieldRecord>();
        {
            std::unique_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // WRITE
            if (m_cache.count(key)) {
                record = m_cache.at(key);
            } else {
                m_cache[key] = record;
            }
        }
        {
            std::lock_guard<std::mutex> lock(record->getMutex());
            record->setField(field, value);
        }
    }

    void setDatabaseRecord(const string& key, const string& field, const string& value, 
        const int64_t& timestamp, const int64_t& expiryTimestamp) { // V2
        std::shared_ptr<FieldRecord> record = std::make_shared<FieldRecord>();
        {
            std::unique_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // WRITE
            if (m_cache.count(key)) {
                record = m_cache.at(key);
            } else {
                m_cache[key] = record;
            }
        }
        {
            std::lock_guard<std::mutex> lock(record->getMutex());
            record->setField(field, value, timestamp, expiryTimestamp);
        }
    }

    string getDatabaseRecord(const string& key, const string& field) { // V1
        return getDatabaseRecord(key, field, 0);
    }

    string getDatabaseRecord(const string& key, const string& field, const int64_t& queriedTimestamp) { // V2
        std::shared_ptr<FieldRecord> record;
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
            if (!m_cache.count(key)) {
                return "";
            }
            record = m_cache.at(key);
        }
        if (record) {
            std::lock_guard<std::mutex> lock(record->getMutex());
            return record->getField(field, queriedTimestamp);
        }
        return "";
    }

    bool deleteDatabaseRecord(const string& key, const string& field) { // V1
        return deleteDatabaseRecord(key, field, 0);
    }

    bool deleteDatabaseRecord(const string& key, const string& field, const int64_t& queriedTimestamp) { // V2
        std::shared_ptr<FieldRecord> record;
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
            if (!m_cache.count(key)) {
                return false;
            }
            record = m_cache.at(key);
        }

        bool isDeleted = false;
        bool isKeyNeedsToBeErased = false;
        {
            std::lock_guard<std::mutex> lock(record->getMutex());
            isDeleted = record->deleteField(field, queriedTimestamp);
            if (isDeleted && !record->getSize()) {
                // If the field map is completely empty after deleting the field, delete the key
                isKeyNeedsToBeErased = true;
            }
        }
        if (isKeyNeedsToBeErased) {
            std::unique_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // WRITE
            m_cache.erase(key);
        }
        return isDeleted;
    }

    string scanDatabaseRecords(const string& key) { // V1
        return scanDatabaseRecords(key, 0);
    }

    string scanDatabaseRecords(const string& key, const int64_t& queriedTimestamp) { // V2
        std::shared_ptr<FieldRecord> record;
        string scanResult = "";
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
            if (!m_cache.count(key)) {
                return scanResult;
            }
            record = m_cache.at(key);
        }
        if (record) {
            std::lock_guard<std::mutex> lock(record->getMutex());
            for (const std::pair<string, std::shared_ptr<ValueRecord>>& fieldMap : record->getFieldValueMap()) {
                std::shared_ptr<ValueRecord> value = fieldMap.second;
                if (value->getValue(queriedTimestamp).size()) {
                    scanResult += fieldMap.first + "(" + fieldMap.second->m_value + "), ";
                }
            }
        }
        if (scanResult.size() >= 2) {
            scanResult.resize(scanResult.size()-2);
        }
        return scanResult;
    }

    string scanDatabaseRecordsByPrefix(const string& key, const string& prefix) { // V1
        return scanDatabaseRecordsByPrefix(key, prefix, 0);
    }

    string scanDatabaseRecordsByPrefix(const string& key, const string& prefix, const int64_t& queriedTimestamp) { // V2
        std::shared_ptr<FieldRecord> record;
        string scanResult = "";
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
            if (!m_cache.count(key)) {
                return scanResult;
            }
            record = m_cache.at(key);
        }
        if (record) {
            std::lock_guard<std::mutex> lock(record->getMutex);
            for (const std::pair<string, std::shared_ptr<ValueRecord>>& fieldMap : record->getFieldValueMap()) {
                string field = fieldMap.first;
                std::shared_ptr<ValueRecord> value = fieldMap.second;
                if (field.substr(0, prefix.length()) == prefix && value->getValue(queriedTimestamp).size()) {
                    scanResult += fieldMap.first + "(" + fieldMap.second->m_value + "), ";
                }
            }
        }
        if (scanResult.size() >= 2) {
            scanResult.resize(scanResult.size()-2);
        }
        return scanResult;
    }


public:
    std::vector<std::string> parseInput(const std::vector<std::vector<std::string>>& queries) {
        std::vector<std::string> results; 
        for (std::vector<std::string> query : queries) {
            string operation = query.at(0);
            if (operation == "SET") {
                setDatabaseRecord(query.at(1), query.at(2), query.at(3));
                results.push_back("");
            } else if (operation == "SET_AT" || operation == "SET_AT_WITH_TTL") {
                int64_t expiryTimestamp = INT64_MAX;
                if (query.size() > 5) {
                    expiryTimestamp = std::stoi(query.at(4)) + std::stoi(query.at(5));
                }
                setDatabaseRecord(query.at(1), query.at(2), query.at(3), std::stoi(query.at(4)), expiryTimestamp);
                results.push_back("");
            } else if (operation == "GET") {
                results.push_back(getDatabaseRecord(query.at(1), query.at(2)));
            } else if (operation == "GET_AT") {
                results.push_back(getDatabaseRecord(query.at(1), query.at(2), std::stoi(query.at(3))));
            } else if (operation == "DELETE") {
                bool isDeleted = deleteDatabaseRecord(query.at(1), query.at(2));
                results.push_back(isDeleted ? "true" : "false");
            } else if (operation == "DELETE_AT") {
                bool isDeleted = deleteDatabaseRecord(query.at(1), query.at(2), std::stoi(query.at(3)));
                results.push_back(isDeleted ? "true" : "false");
            } else if (operation == "SCAN") {
                results.push_back(scanDatabaseRecords(query.at(1)));
            } else if (operation == "SCAN_AT") {
                results.push_back(scanDatabaseRecords(query.at(1), std::stoi(query.at(2))));
            } else if (operation == "SCAN_BY_PREFIX") {
                results.push_back(scanDatabaseRecordsByPrefix(query.at(1), query.at(2)));
            } else if (operation == "SCAN_BY_PREFIX_AT") {
                results.push_back(scanDatabaseRecordsByPrefix(query.at(1), query.at(2), std::stoi(query.at(3))));
            }
        }
        return results;
    }
};

class FileStorageRecord {
private:
    std::unordered_map<string, int64_t> m_fieldValueRecord;
    int64_t m_remainingStorageLimit;
public:
    std::mutex m_mtx;
    
    FileStorageRecord(int64_t storageLimit) : m_remainingStorageLimit(storageLimit) {}

    bool setField(const string& field, const int64_t& value) {
        if (m_remainingStorageLimit < value) {
            return false;
        }
        m_fieldValueRecord[field] = value;
        m_remainingStorageLimit -= value;
        return true;
    }

    bool compressFile(const std::string& field) {
        size_t exactPosition = field.find(".COMPRESSED");
        if (exactPosition != std::string::npos || !m_fieldValueRecord.count(field)) {
            return false;
        }
        string newField = field + ".COMPRESSED";
        if (m_fieldValueRecord.count(newField)) {
            return false;
        }
        
        int64_t value = m_fieldValueRecord[field];
        m_fieldValueRecord.erase(field);
        m_remainingStorageLimit += value;
        return setField(newField, value/2);
    }

    bool decompressFile(const std::string& field) {
        size_t exactPosition = field.find(".COMPRESSED");
        if (exactPosition == std::string::npos || !m_fieldValueRecord.count(field)) {
            return false;
        }
        string newField = field.substr(0, exactPosition - 0);
        if (m_fieldValueRecord.count(newField)) {
            return false;
        }

        int64_t value = m_fieldValueRecord[field];
        m_fieldValueRecord.erase(field);
        m_remainingStorageLimit += value;        
        return setField(newField, value*2);
    }

    bool copyFile(const std::string& oldField, const std::string& newField) {
        if (m_fieldValueRecord.count(newField)) {
            return false;
        }
        int64_t value = m_fieldValueRecord.at(oldField);
        m_fieldValueRecord.erase(oldField);
        m_remainingStorageLimit += value;
        return setField(newField, value);
    }

    string getField(const string& field) {
        if (m_fieldValueRecord.count(field)) {
            return to_string(m_fieldValueRecord.at(field));
        }
        return "";
    }

    string getRemainingStorageLimit() {
        return to_string(m_remainingStorageLimit);
    }
};

class FileStorageManager {
private:
    std::shared_timed_mutex m_sharedStateMtx;
    std::unordered_map<string, std::shared_ptr<FileStorageRecord>> m_storage;
    
    void addUser(const std::string& userId, const int64_t& storageLimit) {
        std::unique_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // WRITE
        if (!m_storage.count(userId)) {
            m_storage[userId] = std::make_shared<FileStorageRecord>(storageLimit);
        }
    }

    string addFileBy(const std::string& userId, const std::string& filePath, const int64_t& fileSize) {
        std::shared_ptr<FileStorageRecord> fileRecord;
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
            if (!m_storage.count(userId)) {
                return ""; // UserID does not exist
            }
            fileRecord = m_storage.at(userId);
        }
        if (fileRecord) {
            std::lock_guard<std::mutex> lock(fileRecord->m_mtx);
            if (fileRecord->setField(filePath, fileSize)) {
                return fileRecord->getRemainingStorageLimit();
            }
        }
        return "";
    }

    string compressFile(const std::string& userId, const std::string& filePath) { 
        std::shared_ptr<FileStorageRecord> fileRecord;
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
            if (!m_storage.count(userId)) {
                return ""; // UserID does not exist
            }
            fileRecord = m_storage.at(userId);
        }
        if (fileRecord) {
            std::lock_guard<std::mutex> lock(fileRecord->m_mtx);
            if (fileRecord->compressFile(filePath)) {
                return fileRecord->getRemainingStorageLimit();
            }
        }
        return "";
    }

    string deCompressFile(const std::string& userId, const std::string& filePath) {
        std::shared_ptr<FileStorageRecord> fileRecord;
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
            if (!m_storage.count(userId)) {
                return ""; // UserID does not exist
            }
            fileRecord = m_storage.at(userId);
        }
        if (fileRecord) {
            std::lock_guard<std::mutex> lock(fileRecord->m_mtx);
            if (fileRecord->decompressFile(filePath)) {
                return fileRecord->getRemainingStorageLimit();
            }
        }
        return "";
    }

    string getFileSize(const std::string& filePath) {
        string fileSize = "";
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
            // O(N)
            for (const std::pair<string, std::shared_ptr<FileStorageRecord>>& record : m_storage) {
                std::lock_guard<std::mutex> lock(record.second->m_mtx);
                fileSize = record.second->getField(filePath);
                if (fileSize != "") {
                    return fileSize;
                }
            }
        }
        return fileSize;
    }

    string copyFile(const std::string& existingFilePath, const std::string& newFilePath) {
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
            // O(N)
            for (const std::pair<string, std::shared_ptr<FileStorageRecord>>& record : m_storage) {
                std::shared_ptr<FileStorageRecord> fileRecord = record.second;
                std::lock_guard<std::mutex> lock(fileRecord->m_mtx);
                if (fileRecord->getField(existingFilePath) != "") {
                    if (fileRecord->copyFile(existingFilePath, newFilePath)) {
                        return fileRecord->getRemainingStorageLimit();
                    }
                }
            }
        }
        return "";
    }

public:
    std::vector<std::string> parseInput(const std::vector<std::vector<std::string>>& queries) {
        std::vector<std::string> results; 
        for (std::vector<std::string> query : queries) {
            string operation = query.at(0);
            if (operation == "ADD_USER") {
                addUser(query.at(1), stoi(query.at(2)));
                results.push_back("");
            } else if (operation == "ADD_FILE_BY") {
                results.push_back(addFileBy(query.at(1), query.at(2), std::stoi(query.at(3))));
            } else if (operation == "COMPRESS_FILE") {
                results.push_back(compressFile(query.at(1), query.at(2)));
            } else if (operation == "DECOMPRESS_FILE") {
                results.push_back(deCompressFile(query.at(1), query.at(2)));
            } else if (operation == "GET_FILE_SIZE") {
                results.push_back(getFileSize(query.at(1)));
            } else if (operation == "COPY_FILE") {
                results.push_back(copyFile(query.at(1), query.at(2)));
            }
        }
        return results;
    }
};

int main() {
    cout << "===== LEVEL 1: Basic Operations =====" << endl;
    {
        DatabaseManager db;
        vector<vector<string>> queries = {
            {"SET", "A", "B", "E"},
            {"SET", "A", "C", "F"},
            {"GET", "A", "B"},
            {"GET", "A", "D"},
            {"DELETE", "A", "B"},
            {"DELETE", "A", "D"}
        };
        
        vector<string> results = db.parseInput(queries);
        vector<string> expected = {"", "", "E", "", "true", "false"};
        
        cout << "Results: ";
        for (const auto& r : results) cout << "\"" << r << "\" ";
        cout << endl;
        cout << "Expected: ";
        for (const auto& e : expected) cout << "\"" << e << "\" ";
        cout << endl;
        cout << "Test " << (results == expected ? "PASSED" : "FAILED") << endl << endl;
    }
    
    cout << "===== LEVEL 2: SCAN Operations =====" << endl;
    {
        DatabaseManager db;
        vector<vector<string>> queries = {
            {"SET", "A", "BC", "E"},
            {"SET", "A", "BD", "F"},
            {"SET", "A", "C", "G"},
            {"SCAN_BY_PREFIX", "A", "B"},
            {"SCAN", "A"},
            {"SCAN_BY_PREFIX", "B", "B"}
        };
        
        vector<string> results = db.parseInput(queries);
        vector<string> expected = {"", "", "", "BC(E), BD(F)", "BC(E), BD(F), C(G)", ""};
        
        cout << "Results: ";
        for (const auto& r : results) cout << "\"" << r << "\" ";
        cout << endl;
        cout << "Expected: ";
        for (const auto& e : expected) cout << "\"" << e << "\" ";
        cout << endl;
        cout << "Test " << (results == expected ? "PASSED" : "FAILED") << endl << endl;
    }
    
    cout << "===== LEVEL 3: TTL Operations (Example 1) =====" << endl;
    {
        DatabaseManager db;
        vector<vector<string>> queries = {
            {"SET_AT_WITH_TTL", "A", "BC", "E", "1", "9"},
            {"SET_AT_WITH_TTL", "A", "BC", "E", "5", "10"},
            {"SET_AT", "A", "BD", "F", "5"},
            {"SCAN_BY_PREFIX_AT", "A", "B", "14"},
            {"SCAN_BY_PREFIX_AT", "A", "B", "15"}
        };
        
        vector<string> results = db.parseInput(queries);
        vector<string> expected = {"", "", "", "BC(E), BD(F)", "BD(F)"};
        
        cout << "Results: ";
        for (const auto& r : results) cout << "\"" << r << "\" ";
        cout << endl;
        cout << "Expected: ";
        for (const auto& e : expected) cout << "\"" << e << "\" ";
        cout << endl;
        cout << "Test " << (results == expected ? "PASSED" : "FAILED") << endl << endl;
    }
    
    cout << "===== LEVEL 3: TTL Operations (Example 2) =====" << endl;
    {
        DatabaseManager db;
        vector<vector<string>> queries = {
            {"SET_AT", "A", "B", "C", "1"},
            {"SET_AT_WITH_TTL", "X", "Y", "Z", "2", "15"},
            {"GET_AT", "X", "Y", "3"},
            {"SET_AT_WITH_TTL", "A", "D", "E", "4", "10"},
            {"SCAN_AT", "A", "13"},
            {"SCAN_AT", "X", "16"},
            {"SCAN_AT", "X", "17"},
            {"DELETE_AT", "X", "Y", "20"}
        };
        
        vector<string> results = db.parseInput(queries);
        vector<string> expected = {"", "", "Z", "", "B(C), D(E)", "Y(Z)", "", "false"};
        
        cout << "Results: ";
        for (const auto& r : results) cout << "\"" << r << "\" ";
        cout << endl;
        cout << "Expected: ";
        for (const auto& e : expected) cout << "\"" << e << "\" ";
        cout << endl;
        cout << "Test " << (results == expected ? "PASSED" : "FAILED") << endl << endl;
    }
    
    cout << "===== LEVEL 4: File Storage Operations =====" << endl;
    {
        FileStorageManager fsm;
        vector<vector<string>> queries = {
            {"ADD_USER", "user1", "1000"},
            {"ADD_USER", "user2", "5000"},
            {"ADD_FILE_BY", "user1", "/dir/file.mp4", "500"},
            {"COMPRESS_FILE", "user2", "/dir/file.mp4"},
            {"COMPRESS_FILE", "user3", "/dir/file.mp4"},
            {"COMPRESS_FILE", "user1", "/folder/non_existing_file"},
            {"COMPRESS_FILE", "user1", "/dir/file.mp4"},
            {"GET_FILE_SIZE", "/dir/file.mp4.COMPRESSED"},
            {"GET_FILE_SIZE", "/dir/file.mp4"},
            {"COPY_FILE", "/dir/file.mp4.COMPRESSED", "/file.mp4.COMPRESSED"},
            {"ADD_FILE_BY", "user1", "/dir/file.mp4", "300"},
            {"DECOMPRESS_FILE", "user1", "/dir/file.mp4.COMPRESSED"},
            {"DECOMPRESS_FILE", "user2", "/dir/file.mp4.COMPRESSED"},
            {"DECOMPRESS_FILE", "user1", "/dir/file.mp4.COMPRESSED"},
            {"DECOMPRESS_FILE", "user1", "/file.mp4.COMPRESSED"}
        };
        
        vector<string> results = fsm.parseInput(queries);
        
        cout << "Results: ";
        for (size_t i = 0; i < results.size(); i++) {
            cout << i << ":\"" << results[i] << "\" ";
        }
        cout << endl;
        
        cout << "Note: Manually verify Level 4 results based on storage calculations" << endl << endl;
    }
    
    cout << "===== All Tests Complete =====" << endl;
    
    return 0;
}
