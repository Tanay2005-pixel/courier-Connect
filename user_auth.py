import re

def sign_up():
    username = input("enter the user name: ")
    file = open("user_auth.txt", "r")
    hash = len(username)%10
    if hash == 0: hash = 10
    for i in range(0,hash):
        line = file.readline()
    
    
    if username in line:
        print("Username is already taken")
        return

    if " " in username:
        print("Username cant have spaces")
        return
    
    file.close()
    
    password = input("Enter the password: ")
    upper = False
    lower = False
    number = False
    for i in password:
        if len(password) < 8:
            print("More than 8 character")
            return
        if (ord(i)>=ord('A') and ord(i)<=ord('Z')):
            upper = True
        if (ord(i)>=ord('a') and ord(i)<=ord('z')):
            lower = True
        if i.isnumeric():
            number = True
    
    if upper == False or lower == False or number == False:
        print("Invalid password")
        return

    with open("user_auth.txt", "r") as file:
        line = file.readlines()
        line[hash-2] = line[hash-2] + " " + username+" - "+password


    with open("user_auth.txt", "w") as f:
        f.writelines(line)
    
    print("successfully username and password safed")

def sign_in():
    username = input("Enter the username: ")
    password = input("Enter the password: ")
    hash = len(username) %10
    if(hash == 0): hash = 10

    with open("user_auth.txt", "r") as f:
        for i in range(0,hash):
            line = f.readline()
        
        new = username+" - "+password
        if new not in line:
            print("Wrong username or password")
        else:
            print(f"Welcome {username}")
