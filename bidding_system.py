from pymongo import MongoClient

class Node:
    def __init__(self, data):
        self.data = data
        self.next = None
        self.branch = None

class bstnode:
    def __init__(self, amount, agent_id):
        self.amount = amount
        self.agent_id = agent_id
        self.left = None
        self.right = None

def insert_bst(root, amount, agent_id):
    if not root:
        return bstnode(amount, agent_id)
    if amount < root.amount:
        root.left = insert_bst(root.left, amount, agent_id)
    else:
        root.right = insert_bst(root.right, amount, agent_id)
    return root

def find_min_bst(root):
    current = root
    while current and current.left:
        current = current.left
    return current

def get_agents_from_mongodb():
    client = MongoClient("mongodb://localhost:27017/")
    db = client['CourierSystem']
    agents_col = db['aggents']
    return list(agents_col.find())

def get_requests():
    client = MongoClient("mongodb://localhost:27017/")
    db = client['CourierSystem']
    requests_col = db['users-requests']
    return list(requests_col.find())

def reconstruct_linked_list(doc):
    sender = Node(doc["sender"]["name"])
    receiver = Node(doc["receiver"]["name"])
    box = Node(doc["material"]["box_count"])
    sender.next = receiver
    receiver.next = box
    sender.branch = Node(doc["sender"]["address"])
    sender.branch.next = Node(doc["sender"]["phone"])
    receiver.branch = Node(doc["receiver"]["address"])
    receiver.branch.next = Node(doc["receiver"]["phone"])
    box.branch = Node(doc["material"]["breakable"])
    return sender

class RequestQueue:
    def __init__(self):
        self.q = []
    def enqueue(self, req_head):
        self.q.append(req_head)
    def dequeue(self):
        return self.q.pop(0) if self.q else None
    def size(self):
        return len(self.q)

def main():
    courier_queue = RequestQueue()
    all_requests = get_requests()
    for doc in all_requests:
        request_head = reconstruct_linked_list(doc)
        courier_queue.enqueue(request_head)

    agents = get_agents_from_mongodb()

    while courier_queue.size() > 0:
        request = courier_queue.dequeue()
        print("\nProcessing new request:")
        curr = request
        labels = ["Sender Name", "Receiver Name", "Box Count"]
        i = 0
        while curr:
            print(f"{labels[i]}: {curr.data}")
            curr = curr.next
            i += 1

        print("Collecting bids...")
        bst_root = None
        agent_bid_count = {str(agent['_id']): 0 for agent in agents}
        for agent in agents:
            agent_id = str(agent['_id'])
            for bid_round in range(2):
                bid_amount = float(input("Enter the bid ammount: "))
                print(f"Agent {agent['username']} bid {bid_amount}")
                bst_root = insert_bst(bst_root, bid_amount, agent_id)
                agent_bid_count[agent_id] += 1

        min_bid_node = find_min_bst(bst_root)
        chosen_agent = next(a for a in agents if str(a['_id']) == min_bid_node.agent_id)
        print(f"\nRequest assigned to agent {chosen_agent['username']} with lowest bid {min_bid_node.amount}\n")

    print("All requests processed.")

if __name__ == "__main__":
    main()
