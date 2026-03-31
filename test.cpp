#include <iostream>
#include <cstring>
#include "MemoryRiver.hpp"

using namespace std;

struct TestData {
    int id;
    char name[20];
    double value;

    TestData() : id(0), value(0.0) {
        memset(name, 0, sizeof(name));
    }

    TestData(int i, const char* n, double v) : id(i), value(v) {
        memset(name, 0, sizeof(name));
        strncpy(name, n, 19);
    }

    bool operator==(const TestData& other) const {
        return id == other.id && strcmp(name, other.name) == 0 && value == other.value;
    }

    void print() const {
        cout << "ID: " << id << ", Name: " << name << ", Value: " << value << endl;
    }
};

int main() {
    // Test basic operations
    cout << "Testing MemoryRiver implementation..." << endl;

    MemoryRiver<TestData, 3> mr("test.dat");
    mr.initialise();

    // Test write_info and get_info
    mr.write_info(100, 1);
    mr.write_info(200, 2);
    mr.write_info(300, 3);

    int tmp;
    mr.get_info(tmp, 1);
    cout << "Info 1: " << tmp << " (expected: 100)" << endl;
    mr.get_info(tmp, 2);
    cout << "Info 2: " << tmp << " (expected: 200)" << endl;
    mr.get_info(tmp, 3);
    cout << "Info 3: " << tmp << " (expected: 300)" << endl;

    // Reset first info for free list
    mr.write_info(0, 1);

    // Test write and read
    TestData t1(1, "Alice", 3.14);
    TestData t2(2, "Bob", 2.71);
    TestData t3(3, "Charlie", 1.41);

    int idx1 = mr.write(t1);
    int idx2 = mr.write(t2);
    int idx3 = mr.write(t3);

    cout << "Written indices: " << idx1 << ", " << idx2 << ", " << idx3 << endl;

    TestData r1, r2, r3;
    mr.read(r1, idx1);
    mr.read(r2, idx2);
    mr.read(r3, idx3);

    cout << "Read data 1: "; r1.print();
    cout << "Read data 2: "; r2.print();
    cout << "Read data 3: "; r3.print();

    // Test update
    TestData t2_updated(2, "Bob_Updated", 9.99);
    mr.update(t2_updated, idx2);
    mr.read(r2, idx2);
    cout << "Updated data 2: "; r2.print();

    // Test delete and space reclamation
    mr.Delete(idx2);

    // Write a new object, should reuse deleted space
    TestData t4(4, "David", 5.55);
    int idx4 = mr.write(t4);
    cout << "New index after delete: " << idx4 << " (should be " << idx2 << " if space reused)" << endl;

    TestData r4;
    mr.read(r4, idx4);
    cout << "Read new data: "; r4.print();

    cout << "Test completed!" << endl;

    return 0;
}
