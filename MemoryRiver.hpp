#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <fstream>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

template<class T, int info_len = 2>
class MemoryRiver {
private:
    /* your code here */
    fstream file;
    string file_name;
    int sizeofT = sizeof(T);

    // We use one extra int at position 0 to store the free list head
    // User's info positions 1 to info_len are mapped to actual file positions 1 to info_len
    // Our internal free list head is at actual position 0
    static const int ACTUAL_INFO_LEN = info_len + 1;
    static const int HEADER_SIZE = ACTUAL_INFO_LEN * sizeof(int);

    // Internal function to get free list head (at position 0)
    int get_free_head() {
        file.open(file_name, std::ios::in);
        int head;
        file.seekg(0);
        file.read(reinterpret_cast<char *>(&head), sizeof(int));
        file.close();
        return head;
    }

    // Internal function to set free list head (at position 0)
    void set_free_head(int head) {
        file.open(file_name, std::ios::in | std::ios::out);
        file.seekp(0);
        file.write(reinterpret_cast<char *>(&head), sizeof(int));
        file.close();
    }

public:
    MemoryRiver() = default;

    MemoryRiver(const string& file_name) : file_name(file_name) {}

    void initialise(string FN = "") {
        if (FN != "") file_name = FN;
        file.open(file_name, std::ios::out);
        int tmp = 0;
        // Write ACTUAL_INFO_LEN integers (including our internal free list head)
        for (int i = 0; i < ACTUAL_INFO_LEN; ++i)
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
    }

    //读出第n个int的值赋给tmp，1_base
    void get_info(int &tmp, int n) {
        if (n > info_len) return;
        /* your code here */
        // Map user's position n to actual position n (1-based to 1-based)
        // Position 0 is reserved for our free list head
        file.open(file_name, std::ios::in);
        file.seekg(n * sizeof(int));
        file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
    }

    //将tmp写入第n个int的位置，1_base
    void write_info(int tmp, int n) {
        if (n > info_len) return;
        /* your code here */
        // Map user's position n to actual position n (1-based to 1-based)
        // Position 0 is reserved for our free list head
        file.open(file_name, std::ios::in | std::ios::out);
        file.seekp(n * sizeof(int));
        file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
    }

    //在文件合适位置写入类对象t，并返回写入的位置索引index
    //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
    //位置索引index可以取为对象写入的起始位置
    int write(T &t) {
        /* your code here */
        // Check if there's a free space in the reclaimed list
        int free_head = get_free_head();

        int index;
        if (free_head == 0) {
            // No free space, append to end of file
            file.open(file_name, std::ios::in | std::ios::out);
            file.seekp(0, std::ios::end);
            index = file.tellp();
            file.write(reinterpret_cast<char *>(&t), sizeofT);
            file.close();
        } else {
            // Use the free space
            index = free_head;
            file.open(file_name, std::ios::in | std::ios::out);

            // Read the next free block pointer from current free block
            int next_free = 0;
            file.seekg(index);
            file.read(reinterpret_cast<char *>(&next_free), sizeof(int));

            // Write the object at this position
            file.seekp(index);
            file.write(reinterpret_cast<char *>(&t), sizeofT);
            file.close();

            // Update the free list head
            set_free_head(next_free);
        }

        return index;
    }

    //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
    void update(T &t, const int index) {
        /* your code here */
        file.open(file_name, std::ios::in | std::ios::out);
        file.seekp(index);
        file.write(reinterpret_cast<char *>(&t), sizeofT);
        file.close();
    }

    //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
    void read(T &t, const int index) {
        /* your code here */
        file.open(file_name, std::ios::in);
        file.seekg(index);
        file.read(reinterpret_cast<char *>(&t), sizeofT);
        file.close();
    }

    //删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
    void Delete(int index) {
        /* your code here */
        // Add this block to the free list
        // The free list is a linked list where each free block stores the address of the next free block

        int old_head = get_free_head();

        // Write the old head pointer to the deleted block
        file.open(file_name, std::ios::in | std::ios::out);
        file.seekp(index);
        file.write(reinterpret_cast<char *>(&old_head), sizeof(int));
        file.close();

        // Update the free list head to point to this newly deleted block
        set_free_head(index);
    }
};


#endif //BPT_MEMORYRIVER_HPP
