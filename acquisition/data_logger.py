import time, datetime, serial, random
from os import remove

# Taking input and defining some variables
name = input('Please enter your name: \n')
age = input('Please enter your age: \n')
now = datetime.datetime.now()

# Defining different test states
States = [
    '1. Pure Disturbance',
    '2. Cognitive Task Only',
    '3. Eyes Closed Only',
    '4. Eyes Closed + Cognitive Task',
    '5. Foam Only',
    '6. Foam + Cognitive Task',
    '7. Foam + Eyes Closed',
    '8. Foam + Eyes Closed + Cognitive Task'
]

# Asking user their choice of test state and confirming it
check = 'N'
while check == 'N' or check == 'n':
    state = int(input('Please select a state from 1 to 8: ')) - 1
    while state < 0 or state > 7:
        state = int(input('Please select a state from 1 to 8: ')) - 1
    check = input('Confirm State as {} with y or n: '.format(States[state]))

# Creating a file for storing user data
data_file = open('data.txt', 'w')
state = States[state]
file_name = str(state) + ", " + name + ".txt"
main_file = open(file_name, 'w')
main_file.write('Name: ' + name + ', Age: ' + age + ' years old, Time: ' + now.strftime("%Y-%m-%d %H:%M:%S") + '\n\n')

# Initiating serial connection
arduino = serial.Serial(port='COM14', baudrate=250000, timeout=0.1)
time.sleep(2)
arduino.reset_input_buffer()
arduino.reset_output_buffer()
time.sleep(0.1)

# Test Function
def run_test():
    trial = 'trial'
    arduino.write(bytes(trial, 'utf-8'))
    time.sleep(2)
    for i in range(3):
        random.seed(time.time())
        command = str(random.randint(2,3))
        arduino.write(bytes(command,  'utf-8'))
        time.sleep(2)

        startTime = time.time()
        timeout = startTime + 5
    
        while time.time() <= timeout:
            try:
                data = arduino.readline().decode('utf-8').strip('\r\n')
                data_file.write(data+'\n')
            except:
                continue
    time.sleep(2)
    command = str(0)
    arduino.write(bytes(command,  'utf-8'))


# Defining a function for reading anything that's left in the buffer
def read_excess_data():    
    if arduino.in_waiting > 0:
        data = arduino.read(arduino.in_waiting()).decode('utf-8').strip('\r\n')
        return data
    return ""
    
    #for i in range(2000):
    #   try:
    #        data = arduino.readline().decode('utf-8').strip('\r\n')
    #        data_file.write(data+'\n')
    #    except:
    #        continue 
    # this way works like shit

# Defininig a function to clean the data in the data file
def clean_file(data_file, main_file):
    data_file.close()
    data_file = open('data.txt', 'r')
    for line in data_file:
        if line.strip():
            main_file.write(line)

    data_file.close()
    main_file.close()

    remove('data.txt')



# Initiating the tests
trial = 0
while (True):
    cmd = int(input('What do you want to do?\n1. Run a test\n2. Exit the program\n'))
    while cmd != 1 and cmd != 2:
        cmd = int(input('Please select an option below:\n 1. Run a test\n2. Exit the program\n'))
    
    match cmd:
        case 1:
            trial += 1
            run_test()
                    
        case 2:
            read_excess_data()
            print('{} trials were done this time.\n'.format(trial))
            arduino.close()
            clean_file(data_file, main_file)
            exit()
            


