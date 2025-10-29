from pymongo import MongoClient


client = MongoClient("mongodb://localhost:27017/")
db = client["CourierSystem"]

agents_collection = db["aggents"]
agencies_collection = db["agencies"]


class Node:
    def __init__(self, data):
        self.data = data
        self.next = None

class LL:
    def __init__(self):
        self.head = None

    def append(self, data):
        new = Node(data)
        if not self.head:
            self.head = new
        else:
            new.next = self.head
            self.head = new

    def search(self, val):
        curr = self.head
        while curr:
            if curr.data == val:
                return curr.data
            curr = curr.next
        return None
    
    def search_usernameAndPass(self, username = None, password = None):
        curr = self.head
        while curr:
            if curr.data["username"] == username:
                return curr.data
            curr = curr.next
        
        curr = self.head
        while curr:
            if curr.data["password"] == password:
                return curr.data
            curr = curr.next
        
        return None
    


    def printlist(self):
        if not self.head:
            print("no data")
            return
        else:
            curr = self.head
            while curr:
                print(curr.data)
                curr = curr.next    

all_aggencies = agencies_collection.find({}, {"agency": 1, "_id": 0})   
all_username = agents_collection.find({}, {"username": 1,"password": 1, "_id": 0})
def sign_up():
    agencies = LL()
    for names in all_aggencies:
        agencies.append(names)
    
    usernames = LL()
    for names in all_username:
        usernames.append(names)
    
    agent_agency = input("Enter the agency name: ")
    agent_id = int(input("Enter the Id: "))
    agent_username = input("enter the user name: ")
    agent_password = input("Enter the password: ")

    if(agencies.search({'agency':agent_agency}) == None) :
        print("Your agency is not registed")
        return False

    if(" " in agent_username): 
        print("username can't have spaces")
        return False
    if(usernames.search_usernameAndPass(agent_username) != None):
        print("username already taken ")
        return  False

    data = {
        "agency": agent_agency,
        "agent_id": agent_id,
        "username": agent_username,
        "password": agent_password
    }

    agents_collection.insert_one(data)
    return True


def sign_in():
    agencies = LL()
    for names in all_aggencies:
        agencies.append(names)
    
    usernames = LL()
    for names in all_username:
        usernames.append(names)
    
    username = input("Enter the username: ")
    password = input("Enter the password: ")

    if(usernames.search_usernameAndPass(username) == None):
        print("Invalid user name")
        return False
    
    if(usernames.search_usernameAndPass(username, password) == None):
        print("Wrong password")
        return False
    
    print(f"Welcome {username}")
    return True
    


def main():
    print("Welcome Agent")
    print("1. Create new Account\n2.Login")
    choice = int(input("Enter the choice: "))
    flag  = False
    while flag != True: 
        if choice == 1:
            flag = sign_up()
        else: 
            flag = sign_in()
    return
