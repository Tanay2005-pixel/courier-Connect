import user_auth
import agent_auth

def verification():
    print("*"*5, "Welcome", "*"*5)
    print("1.Custumer \n2. Delivery Agent")
    opt = int(input("Choice: "))
    if(opt == 1):
        user_auth.main()
    else:
        agent_auth.main()
    
if __name__ == "__main__":
    verification()
