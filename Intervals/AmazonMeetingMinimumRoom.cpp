#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

struct Room {
    int m_roomid = -1;
    int start_time;
    int end_time;
    int meetingsAttended = 1;
    Room(int roomid, int startTime, int endTime) : m_roomid(roomid), start_time(startTime), end_time(endTime) {}
    Room(int startTime, int endTime) : start_time(startTime), end_time(endTime) {}
};

struct Cmp {
    bool operator()(const std::shared_ptr<Room> roomA, const std::shared_ptr<Room> roomB) {
        return roomA->end_time > roomB->end_time; // ascending
    }
};

/**
 * 1. Sort the intervals by start time stamp
 * 2. Create a room object based on first interval and push them to minHeap.
 * 3. Start looping from the second interval and check if the top of the minHeap 
 *    room has end time <= current interval start time.
 *      1. If yes, take the room by popping and then pushing it back.
 *      2. If no, there is no point in scanning all the rooms and create a new room.
 * 4. Return the heap size as we keep on creating new rooms.
 */
int minMeetingRooms(vector<vector<int>>& intervals) {
    if (intervals.empty()) {
        return 0;
    }

    // Sort them by start timestamp
    std::sort(intervals.begin(), intervals.end(), [](const vector<int>& intervalA, const vector<int>& intervalB) {
        return intervalA[0] < intervalB[0]; // ascending
    });

    // Create a room and start the first meeting.
    std::priority_queue<std::shared_ptr<Room>, std::vector<std::shared_ptr<Room>>, Cmp> roomList;
    roomList.push(std::make_shared<Room>(intervals[0].at(0), intervals[0].at(1)));

    for (int currentIndex = 1; currentIndex < intervals.size(); currentIndex++) {
        std::shared_ptr<Room> room = roomList.top();
        // Current interval's start time >= top room's end time, take the room and update the room
        if (intervals.at(currentIndex).at(0) >= room->end_time) {
            roomList.pop();
            room->start_time = intervals.at(currentIndex).at(0);
            room->end_time = intervals.at(currentIndex).at(1);
            roomList.push(room);
        } else {
            // If the top room's end time is > current interval, all the other rooms will have greater end time.
            // So, create a new room and push to minHeap
            roomList.push(std::make_shared<Room>(intervals.at(currentIndex).at(0), intervals.at(currentIndex).at(1)));
        }
    }
    return roomList.size();
}

/**
 * Similar problem as above but here the room size is provided so you cannot create unlimited new room.
 * We will follow the footsteps of 'CarRentalOptimization'
 * 1. Sort the intervals by start time stamp
 * 2. Create a minHeap where you sort by pickup time in ascending.
 * 3. Since you have been given total rooms, we are asked to utilize as less as possible.
 * 4. So, we will populate the minHeap with 1 room for the first interval.
 * 5. We will add new room only if - 
 *      1. The current interval start time < top room's pick up time. Hence, we need a new room.
 *      2. If a new room is allowed to be picked based on total rooms.
 * 6. If neither of those conditions met, we will have to wait. We simulate waiting by taking the top room
 *    and making its start time as top room's end time - simulating that we waited and then took up the room.
 * 
 * https://www.hellointerview.com/community/questions/meeting-rooms-iii/cm5eguhah03v4838o6lf58qge?company=Google&level=SENIOR
 */
int roomsWithMostMeetings(vector<vector<int>>& intervals, int totalRooms) {
    if (intervals.empty()) {
        return -1;
    }

    // Sort them by start timestamp
    std::sort(intervals.begin(), intervals.end(), [](const vector<int>& intervalA, const vector<int>& intervalB) {
        return intervalA[0] < intervalB[0]; // ascending
    });

    // populate the heap with the first room
    std::priority_queue<std::shared_ptr<Room>, std::vector<std::shared_ptr<Room>>, Cmp> roomList;
    roomList.push(std::make_shared<Room>(roomList.size(), intervals.at(0).at(0), intervals.at(0).at(1)));

    int indexWithMostMeetings = -1;
    int maxMeetings = -1;

    for (int currentIndex = 1; currentIndex < intervals.size(); currentIndex++) {
        std::shared_ptr<Room> room = roomList.top();
        // Current interval's start time >= top room's end time, take the room and update the room.
        if (intervals.at(currentIndex).at(0) >= room->end_time) {
            roomList.pop();
            room->start_time = intervals.at(currentIndex).at(0);
            room->end_time = intervals.at(currentIndex).at(1) + room->start_time;
            room->meetingsAttended++;
            roomList.push(room);
        } else if (roomList.size() < totalRooms) {
            // If the top room's end time is > current interval's start time, you can pickup a new room if allowed.
            std::shared_ptr<Room> newRoom = std::make_shared<Room>(roomList.size(), 
                intervals.at(currentIndex).at(0), intervals.at(currentIndex).at(1) + intervals.at(currentIndex).at(0));
            roomList.push(newRoom);
        } else {
            // Unlike previous problem, you can't create another room nor you can find another room
            // as room's are sorted by end timestamp. So, if top room is busy, every other room is busy.
            // So, simulate waiting in else condition by popping the top room and incrementing the start time 
            // to be right after top's room end time (discarding the original interval start time)
            roomList.pop();
            room->start_time = room->end_time;
            room->end_time = intervals.at(currentIndex).at(1) + room->start_time;
            room->meetingsAttended++;
            roomList.push(room);
        }

        maxMeetings = std::max(maxMeetings, room->meetingsAttended);
        if (maxMeetings == room->meetingsAttended) {
            indexWithMostMeetings = room->m_roomid;
        }
    }

    return indexWithMostMeetings;
}

int main() {
    std::vector<std::vector<int>> schedule {{9,10}, {4,9}, {4,17}};
    std::cout << minMeetingRooms(schedule) << std::endl;
    std::vector<std::vector<int>> schedule2 {{0,10}, {1,5}, {2,7}};
    std::cout << roomsWithMostMeetings(schedule2, 2) << std::endl;

}
