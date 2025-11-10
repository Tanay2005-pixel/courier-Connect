def agency_addition():
    agency_name=input("Enter the agency name to add: ")
    file = open("agency.txt", "r")
    hash = len(agency_name)%10
    if hash == 0: hash = 10
    for i in range(0,hash):
        line = file.readline()
    
    file.close()

    if agency_name in line:
        print("Agency already exist")
        return
    
    with open("agency.txt", "r") as file:
        line = file.readlines()
        if not line:
            for i in range(0,hash):
                line.append('\n')

    if line[hash-2]=='\n':
        line[hash-2] = agency_name
    else:
        line[hash-2] = line[hash-2] + " " + agency_name
    

    file.close()

    file=open("agency.txt","w")
    file.writelines(line)
    file.close()
    print("Agent successfully added")




    


    

