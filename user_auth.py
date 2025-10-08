from pymongo import MongoClient
import re
from collections import deque

client = MongoClient("mongodb://localhost:27017/")
db = client["CourierSystem"]
users_collection = db["Users"]

class Node:
    def __init__(self, data):
        self.data = data
        self.next = None

class LinkedList:
    def __init__(self):
        self.head = None

    def append(self, data):
        new_node = Node(data)
        if not self.head:
            self.head = new_node
        else:
            curr = self.head
            while curr.next:
                curr = curr.next
            curr.next = new_node

    def search(self, username):
        curr = self.head
        while curr:
            if curr.data["username"] == username:
                return curr.data
            curr = curr.next
        return None


class Stack:
    def __init__(self):
        self.stack = []

    def push(self, item):
        self.stack.append(item)

    def pop(self):
        if self.stack:
            return self.stack.pop()

    def top(self):
        return self.stack[-1] if self.stack else None


active_users = deque()

def validate_password(password):
    if len(password) < 8:
        return False
    if not re.search(r'[A-Z]', password):
        return False
    if not re.search(r'[a-z]', password):
        return False
    if not re.search(r'[0-9]', password):
        return False
    return True

def sign_up(linked_users):
    username = input("Enter username: ")
    existing = users_collection.find_one({"username": username})
    if existing:
        print("Username already taken!")
        return

    password = input("Enter password: ")
    if not validate_password(password):
        print("Password must contain at least:\n- One uppercase\n- One lowercase\n- One digit\n- Minimum 8 characters")
        return

    user_data = {"username": username, "password": password, "role": "customer"}
    users_collection.insert_one(user_data)
    linked_users.append(user_data)
    print("Sign-up successful!")

def sign_in(linked_users, login_stack):
    username = input("Enter username: ")
    password = input("Enter password: ")

    user = users_collection.find_one({"username": username})
    login_stack.push(username)

    if not user:
        print("User not found!")
        return
    if user["password"] != password:
        print("Incorrect password!")
        return

    print(f" Welcome back, {username}!")
    active_users.append(username)

def main():
    linked_users = LinkedList()
    login_stack = Stack()

    for u in users_collection.find():
        linked_users.append(u)

    print("\n1. Sign Up\n2. Sign In")
    opt = input("Enter choice: ")
    if opt == "1":
        sign_up(linked_users)
    elif opt == "2":
        sign_in(linked_users, login_stack)
    else:
        print("Invalid option!")

if __name__ == "__main__":
    main()
