from pymongo import MongoClient

client = MongoClient('mongodb://localhost:27017/')
db = client['CourierSystem']
requests = db['users-requests']


class Node:
    def __init__(self, data):
        self.data = data
        self.next = None      
        self.branch = None    

def branch_linked_list(items):
    head = Node(items[0])
    curr = head
    for item in items[1:]:
        curr.next = Node(item)
        curr = curr.next
    return head

def build_request_ll():
    sender_name = input("Enter sender's name: ")
    sender_address = input("Enter sender's address: ")
    sender_phone = input("Enter sender's phone number: ")

    receiver_name = input("Enter receiver's name: ")
    receiver_address = input("Enter receiver's address: ")
    receiver_phone = input("Enter receiver's phone number: ")

    box_count = input("Enter number of boxes: ")
    breakable = input("Is the material breakable? (yes/no): ")

    sender_node = Node(sender_name)
    receiver_node = Node(receiver_name)
    material_node = Node(box_count)
    
    sender_node.next = receiver_node
    receiver_node.next = material_node

    sender_node.branch = branch_linked_list([sender_address, sender_phone])
    receiver_node.branch = branch_linked_list([receiver_address, receiver_phone])
    material_node.branch = Node(breakable)


    doc = {
        "sender": {
            "name": sender_name,
            "address": sender_address,
            "phone": sender_phone
        },
        "receiver": {
            "name": receiver_name,
            "address": receiver_address,
            "phone": receiver_phone
        },
        "material": {
            "box_count": box_count,
            "breakable": breakable
        }
    }
    
    requests.insert_one(doc)

    return sender_node

def print_linked_list(head):
    curr = head
    labels = ["Sender Name", "Receiver Name", "Box Count"]
    i = 0
    print("-"*30)
    while curr:
        print(f"{labels[i]}: {curr.data}")
        branch_curr = curr.branch
        branch_labels = []
        if i == 0:
            branch_labels = ["Sender Address", "Sender Phone"]
        elif i == 1:
            branch_labels = ["Receiver Address", "Receiver Phone"]
        elif i == 2:
            branch_labels = ["Breakable"]
        j = 0
        while branch_curr:
            print(f"  {branch_labels[j]}: {branch_curr.data}")
            branch_curr = branch_curr.next
            j += 1
        curr = curr.next
        i += 1
    print("-"*30)



if __name__ == "__main__":
    head_node = build_request_ll()
    print_linked_list(head_node)
