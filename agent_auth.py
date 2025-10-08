from pymongo import MongoClient
import re
from collections import deque

client = MongoClient("mongodb://localhost:27017/")
db = client["CourierSystem"]

agents_collection = db["aggents"]
agencies_collection = db["agencies"]

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
    def push(self, item): self.stack.append(item)
    def pop(self): return self.stack.pop() if self.stack else None

active_agents = deque()

def validate_password(password):
    if len(password) < 8: return False
    if not re.search(r'[A-Z]', password): return False
    if not re.search(r'[a-z]', password): return False
    if not re.search(r'[0-9]', password): return False
    return True

def agency_exists(name):
    return agencies_collection.find_one({"agency": name}) is not None

def agent_sign_up(agent_list):
    agency = input("Enter your agency name: ")
    if not agency_exists(agency):
        print(" This agency is not registered in the database.")
        return

    agent_id = input("Enter your Agent ID: ")
    username = input("Enter username: ")

    existing = agents_collection.find_one({"username": username})
    if existing:
        print(" Username already taken!")
        return

    password = input("Enter password: ")
    if not validate_password(password):
        print(" Password must contain at least:\n- One uppercase\n- One lowercase\n- One number\n- Minimum 8 characters")
        return

    data = {
        "agency": agency,
        "agent_id": agent_id,
        "username": username,
        "password": password
    }

    agents_collection.insert_one(data)
    agent_list.append(data)
    print(f" Agent {username} from {agency} registered successfully!")

def agent_sign_in(agent_list, login_stack):
    username = input("Enter username: ")
    password = input("Enter password: ")

    agent = agents_collection.find_one({"username": username})
    login_stack.push(username)

    if not agent:
        print(" No such agent found!")
        return

    if not agency_exists(agent["agency"]):
        print(" This agent’s agency no longer exists in database!")
        return

    if agent["password"] != password:
        print(" Incorrect password!")
        return

    print(f" Welcome, {username} from {agent['agency']}!")
    active_agents.append(username)

def main():
    print("\n=== Delivery Agent Portal ===")
    print("1. Sign Up\n2. Sign In")
    opt = input("Enter choice: ")

    agent_list = LinkedList()
    login_stack = Stack()

    for a in agents_collection.find():
        agent_list.append(a)

    if opt == "1":
        agent_sign_up(agent_list)
    elif opt == "2":
        agent_sign_in(agent_list, login_stack)
    else:
        print("Invalid option!")

    print("\nActive agents:", list(active_agents))
    print("Recent logins:", login_stack.stack)

if __name__ == "__main__":
    main()
