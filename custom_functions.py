class Stringcust:
    def __init__(self):
        pass

    def length(self, s):
        count = 0
        while True:
            try:
                s[count]
                count += 1
            except:
                break
        return count

    def compare(self, str1, str2):
        i = 0
        len1 = self.length(str1)
        len2 = self.length(str2)
        while i < len1 and i < len2:
            if str1[i] < str2[i]:
                return -1
            if str1[i] > str2[i]:
                return 1
            i += 1
        if len1 < len2:
            return -1
        if len1 > len2:
            return 1
        return 0

    def substring(self, s, start, end=None):
        result = ""
        if end is None:
            end = self.length(s)
        i = start
        while i < end and i < self.length(s):
            result += s[i]
            i += 1
        return result

    def strip_edges(self, s):
        start = 0
        end = self.length(s) - 1
        
        while start <= end and s[start] in [' ', '\t', '\n', '\r']:
            start += 1
        while end >= start and s[end] in [' ', '\t', '\n', '\r']:
            end -= 1
        
        return self.substring(s, start, end + 1)

    def find(self, s, pattern):
        n = self.length(s)
        m = self.length(pattern)

        if m == 0 or m > n:
            return -1
        
        for i in range(n - m + 1):
            match = True
            for j in range(m):
                if s[i + j] != pattern[j]:
                    match = False
                    break
            if match:
                return i
        return -1


class Stack:
    def __init__(self):
        self.items = []
        self._size = 0
    
    def push(self, item):
        """Add an item to the top of the stack."""
        self.items += [item]
        self._size += 1
    
    def pop(self):
        """Remove and return the top item from the stack."""
        if self._size == 0:
            return None  
        item = self.items[-1]
        self.items = self.items[:-1]
        self._size -= 1
        return item
    
    def peek(self):
        """Return the top item without removing it."""
        if self._size == 0:
            return None
        return self.items[-1]
    
    def size(self):
        """Return the number of items in the stack."""
        return self._size


class Queue:
    def __init__(self):
        self.items = []
        self._size = 0
    
    def enqueue(self, item):
        """Add an item to the end of the queue."""
        self.items += [item]
        self._size += 1
    
    def dequeue(self):
        """Remove and return the first item from the queue."""
        if self._size == 0:
            return None 
        item = self.items[0]
        self.items = self.items[1:]
        self._size -= 1
        return item
    
    def front(self):
        """Return the front item without removing it."""
        if self._size == 0:
            return None
        return self.items[0]
    
    def size(self):
        """Return the number of items in the queue."""
        return self._size


class HeapAndUnionFind:
    def __init__(self):
        pass
        
    def prelocate_up(self, arr, size):
        key = size - 1
        temp = arr[key]
        while key > 0 and (arr[(key - 1) // 2] > temp):
            arr[key] = arr[(key - 1) // 2]
            key = (key - 1) // 2
        arr[key] = temp

    def left_child(self, key, size):
        temp = (key * 2 + 1)
        if temp >= size:
            return -1
        return temp

    def right_child(self, key, size):
        temp = (key * 2 + 2)
        if temp >= size:
            return -1
        return temp

    def prelocate_down(self, arr, key, size, test_size):
        min_idx = key
        l_c = self.left_child(key, size)
        r_c = self.right_child(key, size)

        if l_c != -1 and arr[l_c] < arr[min_idx]:
            min_idx = l_c
        if r_c != -1 and arr[r_c] < arr[min_idx]:
            min_idx = r_c

        if min_idx != key:
            temp = arr[min_idx]
            arr[min_idx] = arr[key]
            arr[key] = temp
            i = 0
            print()
            self.prelocate_down(arr, min_idx, size, test_size)

    def heapify(self, arr, size):
        i = (size - 1) // 2
        while i >= 0:
            self.prelocate_down(arr, i, size, size)
            i -= 1
        print("The heapified array is:")
        i = 0
        while i < size:
            print(str(arr[i]) + " ", end="")
            i += 1
        print()

    def heap_sort(self, arr, size):
        self.heapify(arr, size)
        i = size - 1
        while i >= 0:
            temp = arr[i]
            arr[i] = arr[0]
            arr[0] = temp
            self.prelocate_down(arr, 0, i, size)
            i -= 1
        print("The sorted array is:")
        i = 0
        while i < size:
            print(str(arr[i]) + " ", end="")
            i += 1
        print()

    def find(self, parent, size, element):
        if element < 0 or element >= size:
            return float('inf')
        if parent[element] == element:
            return element
        parent[element] = self.find(parent, size, parent[element])
        return parent[element]

    def union_by_rank(self, parent, rank, size, a, b):
        parent_a = self.find(parent, size, a)
        parent_b = self.find(parent, size, b)
        
        if parent_a == parent_b:
            return

        if rank[parent_a] > rank[parent_b]:
            parent[parent_b] = parent_a
        elif rank[parent_a] < rank[parent_b]:
            parent[parent_a] = parent_b
        else:
            parent[parent_a] = parent_b
            rank[parent_b] += 1

    def makeset(self, parent, rank, size):
        i = 0
        while i < size:
            parent[i] = i
            rank[i] = 1
            i += 1

class Graph:
    def __init__(self, vertices):
        """Initialize a graph with a given number of vertices."""
        self.vertices = vertices
        self.adjacency_list = {}
        for i in range(vertices):
            self.adjacency_list[i] = []

    def add_edge(self, u, v, weight=1):
        """Add an edge between vertex u and v with given weight."""
        if u not in self.adjacency_list:
            self.adjacency_list[u] = []
        if v not in self.adjacency_list:
            self.adjacency_list[v] = []
        self.adjacency_list[u].append((v, weight))

    def get_neighbors(self, vertex):
        """Return a list of (neighbor, weight) tuples for the given vertex."""
        return self.adjacency_list.get(vertex, [])

    def print_graph(self):
        """Print the adjacency list representation of the graph."""
        for vertex in self.adjacency_list:
            neighbors = self.adjacency_list[vertex]
            neighbor_str = ""
            for neighbor, weight in neighbors:
                neighbor_str += f"({neighbor}, {weight}) "
            print(f"Vertex {vertex}: {neighbor_str}")