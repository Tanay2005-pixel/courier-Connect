from custom_functions import Stringcust
utils = Stringcust()


bucketdir = "auth_buckets/"
bucketprefix = "bucket_"

logdir = "user_logs/"
logprifix = "log_"

numbuckets = 10
tableSize = 997

indexfiles = "index.txt"
indexsize = 9997
indexprime = 9991
bucketprime = 991

def init_index():
    try:
        with open(indexfiles, "r") as f:
            lines = f.readlines()
    except:
        lines = []

    while len(lines) < indexsize:
        lines.append("\n")

    with open(indexfiles, "w") as f:
        f.writelines(lines)


def index_h1(username):
    return utils.length(username) % indexsize


def index_h2(username):
    step = indexprime - utils.length(username) % indexprime
    if step == 0:
        step = 1
    return step


def parse_index_line(line):
    line = utils.strip_edges(line)
    if utils.length(line) == 0:
        return None
    
    parts = []
    temp = ""

    i = 0
    while i < utils.length(line):
        if line[i] == '|':
            parts.append(temp)
            temp = ""
        else:
            temp += line[i]
        i += 1

    parts.append(temp)

    if len(parts) != 4:
        return None

    return parts[0], int(parts[1]), int(parts[2]), int(parts[3])


def index_insert(username, bucket, slot_0based):
    slot_1based = slot_0based + 1

    h1 = utils.length(username) % indexsize
    step = indexprime - utils.length(username) % indexprime
    if step == 0:
        step = 1
    h2 = step

    with open(indexfiles, "r") as f:
        lines = f.readlines()

    for i in range(indexsize):
        idx = (h1 + i * h2) % indexsize
        parsed = parse_index_line(lines[idx])

        if parsed is None:  
            lines[idx] = f"{username}|{bucket}|{slot_1based}|{slot_1based}\n"
            with open(indexfiles, "w") as fw:
                fw.writelines(lines)
            return True

    return False

def index_search(username):
    init_index() 

    h1 = utils.length(username) % indexsize
    step = indexprime - utils.length(username) % indexprime
    if step == 0:
        step = 1
    h2 = step

    with open(indexfiles, "r") as f:
        lines = f.readlines()

    for i in range(indexsize):
        idx = (h1 + i * h2) % indexsize
        parsed = parse_index_line(lines[idx])

        if parsed is None:
            continue

        stored_user, bucket, slot_1based, log_1based = parsed
        if utils.compare(stored_user, username) == 0:
            return bucket, slot_1based, log_1based

    return None



def ensure_bucket(bucket_name):
    try:
        with open(bucket_name, 'r'):
            return
    except:
        with open(bucket_name, 'w') as f:
            for _ in range(tableSize):
                f.write("\n")


def read_slot(bucket_name, index):
    ensure_bucket(bucket_name)
    with open(bucket_name, "r") as f:
        lines = f.readlines()

    if index < len(lines):
        return utils.strip_edges(lines[index])
    return ""


def write_slot(bucket_name, index, data):
    ensure_bucket(bucket_name)

    with open(bucket_name, "r") as f:
        lines = f.readlines()

    while len(lines) < tableSize:
        lines.append("\n")

    lines[index] = data + "\n"

    with open(bucket_name, "w") as f:
        f.writelines(lines)


def hash1(username):
    return utils.length(username) % tableSize


def hash2(username):
    step = bucketprime - utils.length(username) % bucketprime
    if step == 0:
        step = 1
    return step


def get_bucket_index(username):
    return utils.length(username) % numbuckets

def extract_username(line):
    idx = utils.find(line, " - ")
    if idx == -1:
        return None
    return utils.substring(line, 0, idx)


def extract_password(line):
    idx = utils.find(line, " - ")
    if idx == -1:
        return None
    return utils.substring(line, idx + 3, utils.length(line))


def insert_user(username, password):
    if index_search(username) is not None:
        return False  

    bucket_idx = get_bucket_index(username)
    bucket_name = bucketdir + bucketprefix + str(bucket_idx) + ".txt"

    h1 = hash1(username)
    h2 = hash2(username)

    for i in range(tableSize):
        p = (h1 + i * h2) % tableSize

        slot_data = read_slot(bucket_name, p)

        if utils.length(slot_data) == 0:
            write_slot(bucket_name, p, username + " - " + password)
            index_insert(username, bucket_idx, p)
            return True

    return False

def search_user(username, password):
    pos = index_search(username)
    if pos is None:
        return False

    bucket_idx, slot_1based, _log_1based = pos
    bucket_name = bucketdir + bucketprefix + str(bucket_idx) + ".txt"
    p = slot_1based - 1
    record = read_slot(bucket_name, p)

    stored_user = extract_username(record)
    stored_pass = extract_password(record)

    if stored_user is None or stored_pass is None:
        return False

    return (utils.compare(stored_user, username) == 0 and
            utils.compare(stored_pass, password) == 0)

def search_user_log(username):
    pos = index_search(username)
    if pos is None:
        return None, None

    bucket_idx, _slot_1based, log_1based = pos
    log_name = logdir + logprifix + str(bucket_idx) + ".txt"

    p = log_1based - 1
    record = read_slot(log_name, p)

    if utils.length(record) == 0:
        return log_1based, None

    return log_1based, record


def insert_record(username, entry):
    log_1based, existing = search_user_log(username)
    if log_1based is None:
        return False

    bucket_idx = get_bucket_index(username)
    log_name = logdir + logprifix + str(bucket_idx) + ".txt"

    p = log_1based - 1  # 0-based slot index

    if existing is None or utils.length(existing) == 0:
        new_line = entry
    else:
        new_line = existing + " | " + entry

    write_slot(log_name, p, new_line)
    return True

def sign_up():
    username = utils.strip_edges(input("Enter username: "))
    password = utils.strip_edges(input("Enter password: "))

    if utils.length(password) < 4:
        print("Password must be at least 4 characters")
        return

    if insert_user(username, password):
        print("Registration successful!")
    else:
        print("Username already exists!")


def sign_in():
    username = utils.strip_edges(input("Enter username: "))
    password = utils.strip_edges(input("Enter password: "))

    if search_user(username, password):
        insert_record(username, "SIGN_IN")
        print("Welcome", username)
    else:
        print("Invalid username or password")


def main():
    while True:
        print("\n1. Sign Up\n2. Sign In\n3. Exit")
        choice = utils.strip_edges(input("Enter choice: "))
        if choice == "1":
            sign_up()
        elif choice == "2":
            sign_in()
        elif choice == "3":
            break
        else:
            print("Invalid option")


if __name__ == "__main__":
    init_index()
    main()
