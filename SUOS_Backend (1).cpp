// ============================================================
//  Smart University Operations & Resource Management System
//  DSA Project - Complete C++ Backend
//  Libraries: <iostream> and <fstream> ONLY (No STL)
// ============================================================

#include <iostream>
#include <fstream>

// ============================================================
//  UTILITY: Manual string operations (no <string> STL)
// ============================================================

// Compare two C-strings
bool strEqual(const char* a, const char* b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return false;
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

// Copy src into dst
void strCopy(char* dst, const char* src) {
    int i = 0;
    while (src[i] != '\0') { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

// Get length of C-string
int strLen(const char* s) {
    int len = 0;
    while (s[len] != '\0') len++;
    return len;
}

// ============================================================
//  STUDENT STRUCT
// ============================================================

struct Student {
    int    rollNo;
    char   name[100];
    char   department[50];
    float  cgpa;
};

// ============================================================
//  MODULE 1-A: DYNAMIC ARRAY (Student Records)
//  Why: Supports automatic resizing unlike static arrays.
//  Complexity: Insert O(1) amortized, Search O(n)
// ============================================================

class DynamicArray {
private:
    Student* data;   // Raw pointer — no STL
    int      size;   // Current number of elements
    int      cap;    // Current allocated capacity

    // Double the capacity when full
    void resize() {
        cap *= 2;
        Student* newData = new Student[cap];
        for (int i = 0; i < size; i++) newData[i] = data[i];
        delete[] data;
        data = newData;
        std::cout << "[DynamicArray] Resized to capacity " << cap << "\n";
    }

public:
    DynamicArray() {
        cap  = 4;
        size = 0;
        data = new Student[cap];
    }
    ~DynamicArray() { delete[] data; }

    // Add student at end — O(1) amortized
    void insert(Student s) {
        if (size == cap) resize();
        data[size++] = s;
    }

    // Linear search by roll number — O(n)
    int findByRoll(int roll) {
        for (int i = 0; i < size; i++)
            if (data[i].rollNo == roll) return i;
        return -1;
    }

    // Delete by roll number — O(n) for shift
    bool remove(int roll) {
        int idx = findByRoll(roll);
        if (idx == -1) return false;
        for (int i = idx; i < size - 1; i++) data[i] = data[i + 1];
        size--;
        return true;
    }

    // Update CGPA
    bool updateCGPA(int roll, float newCGPA) {
        int idx = findByRoll(roll);
        if (idx == -1) return false;
        data[idx].cgpa = newCGPA;
        return true;
    }

    void display() {
        std::cout << "\n--- Student Dynamic Array ---\n";
        for (int i = 0; i < size; i++) {
            std::cout << "Roll: " << data[i].rollNo
                      << " | Name: " << data[i].name
                      << " | Dept: " << data[i].department
                      << " | CGPA: " << data[i].cgpa << "\n";
        }
    }

    int getSize() { return size; }
    Student getAt(int i) { return data[i]; }
};

// ============================================================
//  MODULE 1-B: HASH TABLE (O(1) Roll Number Lookup)
//  Why: Array search is O(n). Hash gives O(1) average.
//  Implementation: Chaining with linked list nodes
// ============================================================

struct HashNode {
    Student   student;
    HashNode* next;
    HashNode(Student s) : student(s), next(nullptr) {}
};

class HashTable {
private:
    static const int TABLE_SIZE = 101; // Prime for better distribution
    HashNode* buckets[TABLE_SIZE];

    // Hash function: roll number mod TABLE_SIZE
    int hash(int roll) { return roll % TABLE_SIZE; }

public:
    HashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) buckets[i] = nullptr;
    }
    ~HashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            HashNode* cur = buckets[i];
            while (cur) { HashNode* tmp = cur->next; delete cur; cur = tmp; }
        }
    }

    // Insert — O(1) average
    void insert(Student s) {
        int idx = hash(s.rollNo);
        HashNode* node = new HashNode(s);
        node->next = buckets[idx];
        buckets[idx] = node;
    }

    // Search — O(1) average
    Student* search(int roll) {
        int idx = hash(roll);
        HashNode* cur = buckets[idx];
        while (cur) {
            if (cur->student.rollNo == roll) return &cur->student;
            cur = cur->next;
        }
        return nullptr;
    }

    // Delete — O(1) average
    bool remove(int roll) {
        int idx = hash(roll);
        HashNode* cur = buckets[idx];
        HashNode* prev = nullptr;
        while (cur) {
            if (cur->student.rollNo == roll) {
                if (prev) prev->next = cur->next;
                else       buckets[idx] = cur->next;
                delete cur;
                return true;
            }
            prev = cur; cur = cur->next;
        }
        return false;
    }
};

// ============================================================
//  MODULE 1-C: AVL TREE (Sorted CGPA / Name queries)
//  Why: Plain BST can degrade to O(n) when skewed.
//  AVL maintains O(log n) via rotations.
// ============================================================

struct AVLNode {
    Student  student;
    AVLNode* left;
    AVLNode* right;
    int      height;
    AVLNode(Student s) : student(s), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
private:
    AVLNode* root;

    int height(AVLNode* n) { return n ? n->height : 0; }
    int balanceFactor(AVLNode* n) { return n ? height(n->left) - height(n->right) : 0; }

    void updateHeight(AVLNode* n) {
        int lh = height(n->left), rh = height(n->right);
        n->height = 1 + (lh > rh ? lh : rh);
    }

    // Right rotation
    AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x  = y->left;
        AVLNode* T2 = x->right;
        x->right = y;
        y->left  = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    // Left rotation
    AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y  = x->right;
        AVLNode* T2 = y->left;
        y->left  = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    // Balance node after insert/delete
    AVLNode* balance(AVLNode* n) {
        updateHeight(n);
        int bf = balanceFactor(n);
        // Left heavy
        if (bf > 1) {
            if (balanceFactor(n->left) < 0) n->left = rotateLeft(n->left);
            return rotateRight(n);
        }
        // Right heavy
        if (bf < -1) {
            if (balanceFactor(n->right) > 0) n->right = rotateRight(n->right);
            return rotateLeft(n);
        }
        return n;
    }

    // Insert by CGPA (key)
    AVLNode* insert(AVLNode* node, Student s) {
        if (!node) return new AVLNode(s);
        if (s.cgpa < node->student.cgpa)       node->left  = insert(node->left,  s);
        else if (s.cgpa > node->student.cgpa)  node->right = insert(node->right, s);
        else                                    node->right = insert(node->right, s); // duplicates right
        return balance(node);
    }

    // In-order traversal (ascending CGPA)
    void inOrder(AVLNode* node) {
        if (!node) return;
        inOrder(node->left);
        std::cout << "Roll: " << node->student.rollNo
                  << " | " << node->student.name
                  << " | CGPA: " << node->student.cgpa << "\n";
        inOrder(node->right);
    }

    // Range query: students with CGPA in [lo, hi]
    void rangeQuery(AVLNode* node, float lo, float hi) {
        if (!node) return;
        if (node->student.cgpa > lo) rangeQuery(node->left, lo, hi);
        if (node->student.cgpa >= lo && node->student.cgpa <= hi)
            std::cout << "Roll: " << node->student.rollNo
                      << " | CGPA: " << node->student.cgpa << "\n";
        if (node->student.cgpa < hi) rangeQuery(node->right, lo, hi);
    }

    void deleteTree(AVLNode* node) {
        if (!node) return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

public:
    AVLTree() : root(nullptr) {}
    ~AVLTree() { deleteTree(root); }

    void insert(Student s)              { root = insert(root, s); }
    void displaySorted()                { std::cout << "\n--- Students Sorted by CGPA ---\n"; inOrder(root); }
    void queryRange(float lo, float hi) { std::cout << "\n--- CGPA Range [" << lo << " - " << hi << "] ---\n"; rangeQuery(root, lo, hi); }
};

// ============================================================
//  MODULE 2: SINGLY LINKED LIST (Course Registration)
//  Why: Insertion/deletion O(1) at head; no costly shifting.
// ============================================================

struct CourseNode {
    char       courseCode[20];
    char       courseName[100];
    int        credits;
    CourseNode* next;
};

class SinglyLinkedList {
private:
    CourseNode* head;
    int         count;

public:
    SinglyLinkedList() : head(nullptr), count(0) {}
    ~SinglyLinkedList() {
        CourseNode* cur = head;
        while (cur) { CourseNode* tmp = cur->next; delete cur; cur = tmp; }
    }

    // Add at head — O(1)
    void addCourse(const char* code, const char* name, int credits) {
        CourseNode* node = new CourseNode();
        strCopy(node->courseCode, code);
        strCopy(node->courseName, name);
        node->credits = credits;
        node->next = head;
        head = node;
        count++;
        std::cout << "[CourseList] Added: " << code << "\n";
    }

    // Drop course by code — O(n) to find, O(1) to remove
    bool dropCourse(const char* code) {
        CourseNode* cur = head;
        CourseNode* prev = nullptr;
        while (cur) {
            if (strEqual(cur->courseCode, code)) {
                if (prev) prev->next = cur->next;
                else      head = cur->next;
                delete cur;
                count--;
                std::cout << "[CourseList] Dropped: " << code << "\n";
                return true;
            }
            prev = cur; cur = cur->next;
        }
        return false;
    }

    void display() {
        std::cout << "\n--- Registered Courses ---\n";
        CourseNode* cur = head;
        while (cur) {
            std::cout << cur->courseCode << " | " << cur->courseName
                      << " | " << cur->credits << " credits\n";
            cur = cur->next;
        }
    }

    int getCount() { return count; }
};

// ============================================================
//  MODULE 3-A: QUEUE (FIFO — Library Reservations)
//  Why: Fair order — first request served first.
// ============================================================

class Queue {
private:
    static const int MAX = 200;
    int  data[MAX];
    int  front, rear, size;

public:
    Queue() : front(0), rear(-1), size(0) {}

    bool enqueue(int val) {
        if (size == MAX) { std::cout << "[Queue] Full!\n"; return false; }
        rear = (rear + 1) % MAX;
        data[rear] = val;
        size++;
        return true;
    }

    int dequeue() {
        if (size == 0) { std::cout << "[Queue] Empty!\n"; return -1; }
        int val = data[front];
        front = (front + 1) % MAX;
        size--;
        return val;
    }

    int peek()    { return size > 0 ? data[front] : -1; }
    bool isEmpty(){ return size == 0; }
    int  getSize(){ return size; }

    void display() {
        std::cout << "[Queue] Front→Rear: ";
        for (int i = 0; i < size; i++)
            std::cout << data[(front + i) % MAX] << " ";
        std::cout << "\n";
    }
};

// ============================================================
//  MODULE 4-A: CIRCULAR QUEUE (Token System)
//  Why: Normal queue wastes memory after dequeue.
//  Circular queue reuses freed positions.
// ============================================================

class CircularQueue {
private:
    static const int MAX = 50;
    int  tokens[MAX];
    int  front, rear, size;

public:
    CircularQueue() : front(0), rear(-1), size(0) {}

    bool enqueue(int token) {
        if (size == MAX) { std::cout << "[CQueue] Full!\n"; return false; }
        rear = (rear + 1) % MAX;
        tokens[rear] = token;
        size++;
        return true;
    }

    int dequeue() {
        if (size == 0) { std::cout << "[CQueue] Empty!\n"; return -1; }
        int val = tokens[front];
        front = (front + 1) % MAX;
        size--;
        return val;
    }

    bool isEmpty() { return size == 0; }
    int  getSize() { return size; }

    void display() {
        std::cout << "[CircularQueue] Tokens: ";
        for (int i = 0; i < size; i++)
            std::cout << tokens[(front + i) % MAX] << " ";
        std::cout << "\n";
    }
};

// ============================================================
//  MODULE 4-B: PRIORITY QUEUE / MAX-HEAP (Emergency Handling)
//  Why: Emergency complaints (Fire, Medical) must jump ahead.
//  Heap insert/delete O(log n) vs sorted array O(n).
// ============================================================

struct Task {
    int  priority;   // Higher = more urgent
    char description[200];
};

class PriorityQueueHeap {
private:
    static const int MAX = 100;
    Task heap[MAX];
    int  size;

    void swap(Task& a, Task& b) { Task tmp = a; a = b; b = tmp; }

    // Bubble up after insert
    void heapifyUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (heap[idx].priority > heap[parent].priority) {
                swap(heap[idx], heap[parent]);
                idx = parent;
            } else break;
        }
    }

    // Bubble down after remove
    void heapifyDown(int idx) {
        while (true) {
            int largest = idx;
            int left  = 2 * idx + 1;
            int right = 2 * idx + 2;
            if (left  < size && heap[left].priority  > heap[largest].priority) largest = left;
            if (right < size && heap[right].priority > heap[largest].priority) largest = right;
            if (largest != idx) { swap(heap[idx], heap[largest]); idx = largest; }
            else break;
        }
    }

public:
    PriorityQueueHeap() : size(0) {}

    // Insert — O(log n)
    bool insert(int priority, const char* desc) {
        if (size == MAX) return false;
        heap[size].priority = priority;
        strCopy(heap[size].description, desc);
        heapifyUp(size);
        size++;
        return true;
    }

    // Remove max priority — O(log n)
    Task extractMax() {
        Task top = heap[0];
        heap[0] = heap[--size];
        heapifyDown(0);
        return top;
    }

    Task peek()     { return heap[0]; }
    bool isEmpty()  { return size == 0; }
    int  getSize()  { return size; }

    void display() {
        std::cout << "\n--- Priority Queue (Emergency Tasks) ---\n";
        for (int i = 0; i < size; i++)
            std::cout << "P[" << heap[i].priority << "] " << heap[i].description << "\n";
    }
};

// ============================================================
//  MODULE 6: STACK (Undo / Redo System)
//  Why: LIFO = last action undone first. Queue would be wrong.
// ============================================================

struct Action {
    char type[50];       // "ADD_STUDENT", "DELETE_STUDENT", etc.
    char data[300];      // Serialized data for rollback
};

class Stack {
private:
    static const int MAX = 200;
    Action items[MAX];
    int    top;

public:
    Stack() : top(-1) {}

    bool push(const char* type, const char* data) {
        if (top == MAX - 1) { std::cout << "[Stack] Overflow!\n"; return false; }
        top++;
        strCopy(items[top].type, type);
        strCopy(items[top].data, data);
        return true;
    }

    Action pop() {
        Action empty; strCopy(empty.type, "EMPTY"); strCopy(empty.data, "");
        if (top == -1) { std::cout << "[Stack] Underflow!\n"; return empty; }
        return items[top--];
    }

    Action peek() { return items[top]; }
    bool isEmpty(){ return top == -1; }
    int  getSize(){ return top + 1; }

    void display() {
        std::cout << "\n--- Stack Contents ---\n";
        for (int i = top; i >= 0; i--)
            std::cout << "[" << i << "] " << items[i].type << " | " << items[i].data << "\n";
    }
};

// ============================================================
//  MODULE 7: DOUBLY LINKED LIST (Recently Viewed Records)
//  Why: O(1) forward AND backward navigation.
//  Singly LL needs O(n) to go back.
// ============================================================

struct HistoryNode {
    int          rollNo;
    char         name[100];
    HistoryNode* prev;
    HistoryNode* next;
};

class DoublyLinkedHistory {
private:
    HistoryNode* head;
    HistoryNode* tail;
    HistoryNode* current;
    int          size;
    static const int MAX_HISTORY = 20;

    void trimHistory() {
        while (size > MAX_HISTORY && head) {
            HistoryNode* tmp = head;
            head = head->next;
            if (head) head->prev = nullptr;
            delete tmp;
            size--;
        }
    }

public:
    DoublyLinkedHistory() : head(nullptr), tail(nullptr), current(nullptr), size(0) {}
    ~DoublyLinkedHistory() {
        HistoryNode* cur = head;
        while (cur) { HistoryNode* tmp = cur->next; delete cur; cur = tmp; }
    }

    // Add viewed record — O(1)
    void view(int roll, const char* name) {
        HistoryNode* node = new HistoryNode();
        node->rollNo = roll;
        strCopy(node->name, name);
        node->prev = tail;
        node->next = nullptr;
        if (tail) tail->next = node;
        else      head = node;
        tail = node;
        current = node;
        size++;
        trimHistory();
    }

    // Go back — O(1)
    HistoryNode* goBack() {
        if (current && current->prev) {
            current = current->prev;
            return current;
        }
        std::cout << "[History] No previous record.\n";
        return nullptr;
    }

    // Go forward — O(1)
    HistoryNode* goForward() {
        if (current && current->next) {
            current = current->next;
            return current;
        }
        std::cout << "[History] No next record.\n";
        return nullptr;
    }

    void display() {
        std::cout << "\n--- Viewing History (oldest → newest) ---\n";
        HistoryNode* cur = head;
        while (cur) {
            std::cout << (cur == current ? ">> " : "   ")
                      << "Roll: " << cur->rollNo << " | " << cur->name << "\n";
            cur = cur->next;
        }
    }
};

// ============================================================
//  MODULE 5: CAMPUS GRAPH (Adjacency List)
//  Why: Adjacency list O(V+E) vs matrix O(V²). Campus is sparse.
// ============================================================

struct EdgeNode {
    int       dest;
    int       weight;  // Distance in meters
    EdgeNode* next;
};

class CampusGraph {
private:
    static const int MAX_VERTICES = 20;
    char      buildingNames[MAX_VERTICES][100];
    EdgeNode* adjList[MAX_VERTICES];
    int       numVertices;

    // Min-heap for Dijkstra
    struct HeapEntry { int vertex; int dist; };
    HeapEntry dijkHeap[MAX_VERTICES * MAX_VERTICES];
    int       heapSize;

    void heapPush(int v, int d) {
        dijkHeap[heapSize] = {v, d};
        int i = heapSize++;
        while (i > 0) {
            int p = (i - 1) / 2;
            if (dijkHeap[i].dist < dijkHeap[p].dist) {
                HeapEntry tmp = dijkHeap[i]; dijkHeap[i] = dijkHeap[p]; dijkHeap[p] = tmp;
                i = p;
            } else break;
        }
    }

    HeapEntry heapPop() {
        HeapEntry top = dijkHeap[0];
        dijkHeap[0] = dijkHeap[--heapSize];
        int i = 0;
        while (true) {
            int s = i, l = 2*i+1, r = 2*i+2;
            if (l < heapSize && dijkHeap[l].dist < dijkHeap[s].dist) s = l;
            if (r < heapSize && dijkHeap[r].dist < dijkHeap[s].dist) s = r;
            if (s != i) {
                HeapEntry tmp = dijkHeap[i]; dijkHeap[i] = dijkHeap[s]; dijkHeap[s] = tmp;
                i = s;
            } else break;
        }
        return top;
    }

public:
    CampusGraph() : numVertices(0), heapSize(0) {
        for (int i = 0; i < MAX_VERTICES; i++) adjList[i] = nullptr;
    }
    ~CampusGraph() {
        for (int i = 0; i < numVertices; i++) {
            EdgeNode* cur = adjList[i];
            while (cur) { EdgeNode* tmp = cur->next; delete cur; cur = tmp; }
        }
    }

    // Add building (vertex)
    int addBuilding(const char* name) {
        if (numVertices >= MAX_VERTICES) return -1;
        strCopy(buildingNames[numVertices], name);
        return numVertices++;
    }

    // Add road (edge, undirected)
    void addRoad(int u, int v, int weight) {
        EdgeNode* e1 = new EdgeNode{v, weight, adjList[u]}; adjList[u] = e1;
        EdgeNode* e2 = new EdgeNode{u, weight, adjList[v]}; adjList[v] = e2;
    }

    // BFS — Unweighted shortest path — O(V+E)
    void bfs(int start) {
        bool visited[MAX_VERTICES] = {};
        int  bfsQueue[MAX_VERTICES];
        int  qFront = 0, qRear = 0;
        visited[start] = true;
        bfsQueue[qRear++] = start;
        std::cout << "\n--- BFS from " << buildingNames[start] << " ---\n";
        while (qFront < qRear) {
            int u = bfsQueue[qFront++];
            std::cout << buildingNames[u] << " ";
            EdgeNode* e = adjList[u];
            while (e) {
                if (!visited[e->dest]) {
                    visited[e->dest] = true;
                    bfsQueue[qRear++] = e->dest;
                }
                e = e->next;
            }
        }
        std::cout << "\n";
    }

    // Dijkstra — Weighted shortest paths — O((V+E) log V)
    void dijkstra(int src) {
        int dist[MAX_VERTICES];
        bool done[MAX_VERTICES] = {};
        for (int i = 0; i < numVertices; i++) dist[i] = 999999;
        dist[src] = 0;
        heapSize = 0;
        heapPush(src, 0);
        std::cout << "\n--- Dijkstra from " << buildingNames[src] << " ---\n";
        while (heapSize > 0) {
            HeapEntry cur = heapPop();
            int u = cur.vertex;
            if (done[u]) continue;
            done[u] = true;
            EdgeNode* e = adjList[u];
            while (e) {
                int v = e->dest, w = e->weight;
                if (!done[v] && dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    heapPush(v, dist[v]);
                }
                e = e->next;
            }
        }
        for (int i = 0; i < numVertices; i++)
            std::cout << buildingNames[src] << " -> " << buildingNames[i]
                      << ": " << dist[i] << "m\n";
    }

    void display() {
        std::cout << "\n--- Campus Graph (Adjacency List) ---\n";
        for (int i = 0; i < numVertices; i++) {
            std::cout << buildingNames[i] << ": ";
            EdgeNode* e = adjList[i];
            while (e) { std::cout << buildingNames[e->dest] << "(" << e->weight << "m) "; e = e->next; }
            std::cout << "\n";
        }
    }
};

// ============================================================
//  MODULE 8: SORTING ENGINE (Manual Implementations)
//  All operate on int arrays for demonstration / ranking
// ============================================================

class SortingEngine {
public:
    // Bubble Sort — O(n²)
    static void bubbleSort(int arr[], int n) {
        for (int i = 0; i < n - 1; i++)
            for (int j = 0; j < n - i - 1; j++)
                if (arr[j] > arr[j + 1]) { int t = arr[j]; arr[j] = arr[j+1]; arr[j+1] = t; }
    }

    // Insertion Sort — O(n²)
    static void insertionSort(int arr[], int n) {
        for (int i = 1; i < n; i++) {
            int key = arr[i], j = i - 1;
            while (j >= 0 && arr[j] > key) { arr[j + 1] = arr[j]; j--; }
            arr[j + 1] = key;
        }
    }

    // Merge Sort — O(n log n)
    static void merge(int arr[], int l, int m, int r) {
        int n1 = m - l + 1, n2 = r - m;
        int* L = new int[n1]; int* R = new int[n2];
        for (int i = 0; i < n1; i++) L[i] = arr[l + i];
        for (int i = 0; i < n2; i++) R[i] = arr[m + 1 + i];
        int i = 0, j = 0, k = l;
        while (i < n1 && j < n2) arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
        delete[] L; delete[] R;
    }
    static void mergeSort(int arr[], int l, int r) {
        if (l < r) { int m = (l + r) / 2; mergeSort(arr, l, m); mergeSort(arr, m+1, r); merge(arr, l, m, r); }
    }

    // Quick Sort — O(n log n) average
    static int partition(int arr[], int low, int high) {
        int pivot = arr[high], i = low - 1;
        for (int j = low; j < high; j++)
            if (arr[j] <= pivot) { i++; int t = arr[i]; arr[i] = arr[j]; arr[j] = t; }
        int t = arr[i+1]; arr[i+1] = arr[high]; arr[high] = t;
        return i + 1;
    }
    static void quickSort(int arr[], int low, int high) {
        if (low < high) { int p = partition(arr, low, high); quickSort(arr, low, p-1); quickSort(arr, p+1, high); }
    }

    // Heap Sort — O(n log n)
    static void heapify(int arr[], int n, int i) {
        int largest = i, l = 2*i+1, r = 2*i+2;
        if (l < n && arr[l] > arr[largest]) largest = l;
        if (r < n && arr[r] > arr[largest]) largest = r;
        if (largest != i) { int t = arr[i]; arr[i] = arr[largest]; arr[largest] = t; heapify(arr, n, largest); }
    }
    static void heapSort(int arr[], int n) {
        for (int i = n/2 - 1; i >= 0; i--) heapify(arr, n, i);
        for (int i = n - 1; i > 0; i--) { int t = arr[0]; arr[0] = arr[i]; arr[i] = t; heapify(arr, i, 0); }
    }

    static void printArray(int arr[], int n) {
        for (int i = 0; i < n; i++) std::cout << arr[i] << " ";
        std::cout << "\n";
    }
};

// ============================================================
//  MODULE 8: SEARCHING ENGINE
// ============================================================

class SearchingEngine {
public:
    // Linear Search — O(n)
    static int linearSearch(int arr[], int n, int key) {
        for (int i = 0; i < n; i++) if (arr[i] == key) return i;
        return -1;
    }

    // Binary Search — O(log n) — requires sorted array
    static int binarySearch(int arr[], int n, int key) {
        int lo = 0, hi = n - 1;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (arr[mid] == key)      return mid;
            else if (arr[mid] < key)  lo = mid + 1;
            else                      hi = mid - 1;
        }
        return -1;
    }
};

// ============================================================
//  FILE MANAGER — Persistent Storage using <fstream>
//  Schema: students.txt | complaints.txt | courses.txt
// ============================================================

class FileManager {
public:
    // Save students from DynamicArray to file
    static void saveStudents(DynamicArray& arr) {
        std::ofstream file("students.txt");
        if (!file) { std::cout << "[FileManager] Cannot open students.txt\n"; return; }
        int n = arr.getSize();
        file << n << "\n";
        for (int i = 0; i < n; i++) {
            Student s = arr.getAt(i);
            file << s.rollNo << "\n"
                 << s.name   << "\n"
                 << s.department << "\n"
                 << s.cgpa   << "\n";
        }
        file.close();
        std::cout << "[FileManager] Saved " << n << " students.\n";
    }

    // Load students from file into DynamicArray + HashTable + AVLTree
    static void loadStudents(DynamicArray& arr, HashTable& ht, AVLTree& avl) {
        std::ifstream file("students.txt");
        if (!file) { std::cout << "[FileManager] students.txt not found.\n"; return; }
        int n; file >> n; file.ignore();
        for (int i = 0; i < n; i++) {
            Student s;
            file >> s.rollNo; file.ignore();
            file.getline(s.name, 100);
            file.getline(s.department, 50);
            file >> s.cgpa; file.ignore();
            arr.insert(s);
            ht.insert(s);
            avl.insert(s);
        }
        file.close();
        std::cout << "[FileManager] Loaded " << n << " students.\n";
    }

    // Save complaint log
    static void saveComplaint(const char* text, int priority) {
        std::ofstream file("complaints.txt", std::ios::app);
        if (!file) return;
        file << priority << "|" << text << "\n";
        file.close();
    }

    // Display all saved complaints
    static void loadComplaints() {
        std::ifstream file("complaints.txt");
        if (!file) { std::cout << "[FileManager] No complaints file.\n"; return; }
        char line[300];
        std::cout << "\n--- Saved Complaints ---\n";
        while (file.getline(line, 300)) std::cout << line << "\n";
        file.close();
    }
};

// ============================================================
//  MANAGER FACADES
// ============================================================

class StudentManager {
public:
    DynamicArray arr;
    HashTable    ht;
    AVLTree      avl;
    Stack        undoStack;
    Stack        redoStack;

    void addStudent(int roll, const char* name, const char* dept, float cgpa) {
        Student s;
        s.rollNo = roll;
        strCopy(s.name, name);
        strCopy(s.department, dept);
        s.cgpa = cgpa;
        arr.insert(s);
        ht.insert(s);
        avl.insert(s);
        // Log to undo stack
        char data[300];
        // Simple serialization: "roll|name|dept|cgpa"
        // (full serialize omitted for brevity; store rollNo as key)
        undoStack.push("ADD_STUDENT", name);
        std::cout << "[StudentManager] Added: " << name << " (Roll: " << roll << ")\n";
    }

    void searchByRoll(int roll) {
        Student* s = ht.search(roll);
        if (s) std::cout << "Found: " << s->name << " | " << s->department << " | CGPA: " << s->cgpa << "\n";
        else   std::cout << "Not found: Roll " << roll << "\n";
    }

    void save() { FileManager::saveStudents(arr); }
    void load() { FileManager::loadStudents(arr, ht, avl); }
};

class ComplaintManager {
public:
    Queue            normalQueue;
    CircularQueue    tokenQueue;
    PriorityQueueHeap emergencyHeap;

    void addNormal(int id) {
        normalQueue.enqueue(id);
        std::cout << "[Complaint] Normal complaint #" << id << " added.\n";
    }
    void addToken(int token) {
        tokenQueue.enqueue(token);
    }
    void addEmergency(int priority, const char* desc) {
        emergencyHeap.insert(priority, desc);
        FileManager::saveComplaint(desc, priority);
        std::cout << "[Emergency] Added: " << desc << " (P=" << priority << ")\n";
    }
    void processNext() {
        if (!emergencyHeap.isEmpty()) {
            Task t = emergencyHeap.extractMax();
            std::cout << "[EMERGENCY PROCESSED] " << t.description << "\n";
        } else if (!normalQueue.isEmpty()) {
            std::cout << "[Normal PROCESSED] Complaint #" << normalQueue.dequeue() << "\n";
        } else {
            std::cout << "[ComplaintManager] No pending complaints.\n";
        }
    }
};

// ============================================================
//  MAIN — DEMONSTRATION
// ============================================================

int main() {
    std::cout << "==============================================\n";
    std::cout << "  Smart University Operations System (SUOS)  \n";
    std::cout << "==============================================\n\n";

    // --- Student Module ---
    StudentManager sm;
    sm.addStudent(1001, "Ali Hassan",   "CS",    3.8f);
    sm.addStudent(1002, "Sara Khan",    "EE",    3.5f);
    sm.addStudent(1003, "Usman Iqbal",  "ME",    2.9f);
    sm.addStudent(1004, "Zara Ahmed",   "CS",    3.95f);
    sm.addStudent(1005, "Bilal Malik",  "BBA",   3.1f);

    sm.arr.display();
    std::cout << "\n[HashTable Search] Roll 1003: ";
    sm.searchByRoll(1003);
    sm.avl.displaySorted();
    sm.avl.queryRange(3.0f, 3.9f);

    // --- Course Module ---
    SinglyLinkedList courseList;
    courseList.addCourse("CS301", "Data Structures",      3);
    courseList.addCourse("CS302", "Algorithms",           3);
    courseList.addCourse("EE201", "Circuit Analysis",     4);
    courseList.addCourse("MTH101","Calculus II",          3);
    courseList.display();
    courseList.dropCourse("EE201");
    courseList.display();

    // --- Complaint Module ---
    ComplaintManager comp;
    comp.addNormal(101);
    comp.addNormal(102);
    comp.addToken(501);
    comp.addEmergency(10, "FIRE in Lab Block C");
    comp.addEmergency(8,  "Medical Emergency — Student collapsed");
    comp.addEmergency(5,  "Security Alert — Gate 3");
    comp.emergencyHeap.display();
    comp.processNext();  // Should handle FIRE first
    comp.processNext();  // Medical
    comp.processNext();  // Security

    // --- Campus Navigation ---
    CampusGraph campus;
    int lib   = campus.addBuilding("Library");
    int main_ = campus.addBuilding("Main Block");
    int cs    = campus.addBuilding("CS Department");
    int canteen = campus.addBuilding("Canteen");
    int gate  = campus.addBuilding("Main Gate");
    campus.addRoad(lib,  main_, 200);
    campus.addRoad(main_, cs,   150);
    campus.addRoad(main_, canteen, 100);
    campus.addRoad(cs,   gate,  300);
    campus.addRoad(canteen, gate, 250);
    campus.display();
    campus.bfs(gate);
    campus.dijkstra(gate);

    // --- Undo/Redo ---
    Stack undoStack, redoStack;
    undoStack.push("ADD_STUDENT", "Ali Hassan | Roll 1001");
    undoStack.push("ADD_STUDENT", "Sara Khan  | Roll 1002");
    undoStack.push("DELETE_COURSE", "EE201");
    undoStack.display();
    std::cout << "\n[Undo] ";
    Action a = undoStack.pop();
    std::cout << "Undoing: " << a.type << " | " << a.data << "\n";
    redoStack.push(a.type, a.data);

    // --- History ---
    DoublyLinkedHistory history;
    history.view(1001, "Ali Hassan");
    history.view(1002, "Sara Khan");
    history.view(1003, "Usman Iqbal");
    history.display();
    std::cout << "\n[History] Going back: ";
    HistoryNode* node = history.goBack();
    if (node) std::cout << node->name << "\n";
    history.display();

    // --- Sorting Engine ---
    int arr[] = {64, 25, 12, 22, 11, 90, 43};
    int n = 7;
    std::cout << "\n--- Sorting Demo ---\n";
    int a1[7], a2[7], a3[7], a4[7], a5[7];
    for (int i=0;i<n;i++) { a1[i]=a2[i]=a3[i]=a4[i]=a5[i]=arr[i]; }

    std::cout << "Original:   "; SortingEngine::printArray(arr, n);
    SortingEngine::bubbleSort(a1, n);
    std::cout << "Bubble:     "; SortingEngine::printArray(a1, n);
    SortingEngine::insertionSort(a2, n);
    std::cout << "Insertion:  "; SortingEngine::printArray(a2, n);
    SortingEngine::mergeSort(a3, 0, n-1);
    std::cout << "Merge:      "; SortingEngine::printArray(a3, n);
    SortingEngine::quickSort(a4, 0, n-1);
    std::cout << "Quick:      "; SortingEngine::printArray(a4, n);
    SortingEngine::heapSort(a5, n);
    std::cout << "Heap:       "; SortingEngine::printArray(a5, n);

    // --- Binary Search ---
    int key = 25;
    int idx = SearchingEngine::binarySearch(a3, n, key);
    std::cout << "\n[BinarySearch] Key " << key << " found at index: " << idx << "\n";

    // --- File Persistence ---
    sm.save();
    std::cout << "\n[File] Reloading from disk...\n";
    StudentManager sm2;
    sm2.load();
    sm2.arr.display();

    std::cout << "\n=== SUOS Demo Complete ===\n";
    return 0;
}
