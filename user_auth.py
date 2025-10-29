from pymongo import MongoClient
import re



client = MongoClient("mongodb://localhost:27017/")
db = client["CourierSystem"]
users_collection = db["Users"]


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
            if curr.data["username"] == val:
                return curr.data
            curr = curr.next
        return None
    
    def search_usernameAndPass(self, username, password):
        curr = self.head
        while curr:
            if curr.data["username"] == username and curr.data["password"] == password:
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


all_users = users_collection.find({}, {"_id": 0})

def sign_up():
    users = LL()
    for names in all_users:
        users.append(names)
    
    user_name = input("enter the user name: ")
    user_password = input("Enter the password: ")

    if " " in user_name:
        print("User name cant have space")
        return  False
    
    if users.search(user_name) != None: 
        print( "User name Already Taken")
        return False
    
    if len(user_password) < 8:
        print( "Password should have more than 8 character")
        return  False
    
    if not re.search(r'[A-Z]', user_password):
        print( "Password must contain Capital Character")
        return False
     
    if not re.search(r'[a-z]', user_password):
        print( "Password must contain Lower case character")
        return False
    if not re.search(r'[0-9]', user_password):
        print( "Password must contain Digits")
        return False
    
    confirm = input("Enter the password again: ")
    if user_password != confirm:
        print( "Wrong Password")
     
    

    data = {
        "username": user_name,
        "password": user_password
    }

    users_collection.insert_one(data)
    return True

def sign_in():
    users = LL()
    for names in all_users:
        users.append(names)

    username = input("Enter the username: ")
    password = input("Enter the password: ")

    user_exists = False
    curr = users.head
    while curr:
        if curr.data["username"] == username:
            user_exists = True
            break
        curr = curr.next

    if not user_exists:
        print("invalid username")
        return False

    if users.search_usernameAndPass(username, password) is None:
        print("Wrong password")
        return False

    print(f"Welcome {username}")
    return True


def main():
    print("Welcome user")
    print("1. Create new Account\n2.Login")
    choice = int(input("Enter the choice: "))
    flag = False
    while flag != True: 
        if choice == 1:
            flag = sign_up()
        else: 
            flag = sign_in()
    return
