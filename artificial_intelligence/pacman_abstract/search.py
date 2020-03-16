from world import State
from copy import deepcopy
from typing import List, Tuple
import time
from sys import setrecursionlimit

setrecursionlimit(10**6)

def printFront(front: List[State]):
    print('----------FRONT----------')
    for i, state in enumerate(front):
        print(f'Front[{i}]')
        print(state)
        print() # print a new line


def printQueue(queue: List[List[State]]):
    print('----------QUEUE----------')
    for i, l in enumerate(queue):
        print(f'Queue{i}')
        for state in l:
            print(state)
        print()


def findChildren(state: State) -> list:
    children = []

    eat_state = deepcopy(state)
    right_state = deepcopy(state)
    left_state = deepcopy(state)
    up_state = deepcopy(state)
    down_state = deepcopy(state)

    eat_state.eat()
    right_state.moveRight()
    left_state.moveLeft()
    up_state.moveUp()
    down_state.moveDown()

    if state != right_state:
        children.append(right_state)
    if state != left_state:
        children.append(left_state)
    if state != up_state:
        children.append(up_state)
    if state != down_state:
        children.append(down_state)
    if state != eat_state:
        children.append(eat_state)

    return children


def expandFront(front: List[State], method: str = 'DFS'):
    if method == 'DFS':
        if front:
            # printFront(front)
            node = front.pop(0)
            for child in findChildren(node):
                front.insert(0, child)
    elif method == 'BFS':
        if front:
            # printFront(front)
            node = front.pop(0)
            for child in findChildren(node):
                front.append(child)
    elif method == 'BestFS':
        if front:
            # printFront(front)
            node = front.pop(0)
            for child in findChildren(node):
                front.append(child)
            
            # to sort the front we use as a key the number of fruits left and if they are the same
            # we use the steps to the closest fruit
            front.sort(key=lambda state: (len(state.fruits_locations), state.distanceToClosestFruit()))


def expandQueue(queue: List[List[State]], method: str = 'DFS'):
    if method == 'DFS':
        if queue:
            node = queue.pop(0)
            for child in findChildren(node[-1]):
                path = deepcopy(node)
                path.append(child)
                queue.insert(0, path)
    elif method == 'BFS':
        if queue:
            node = queue.pop(0)
            for child in findChildren(node[-1]):
                path = deepcopy(node)
                path.append(child)
                queue.append(path)
    elif method == 'BestFS':
        if queue:
            node = queue.pop(0)
            for child in findChildren(node[-1]):
                path = deepcopy(node)
                path.append(child)
                queue.append(path)
            # same as the front sort
            queue.sort(key=lambda l: (len(l[-1].fruits_locations), l[-1].distanceToClosestFruit()))


def findSolution(front: List[State], queue: List[List[State]], closed: List[State] = [], method: str = 'DFS'):
    if not front:
        print('----------No Solution Found----------')
    elif front[0].reachedGoal():
        print('----------Goal Reached----------')
        for i, state in enumerate(queue[0]):
            print(f'move {i}')
            print(state)
    elif front[0] in closed:
        front_copy = deepcopy(front)
        front_copy.pop(0)
        queue_copy = deepcopy(queue)
        queue_copy.pop(0)
        findSolution(front_copy, queue_copy, closed, method)
    else:
        closed.append(front[0])
        front_copy = deepcopy(front)
        expandFront(front_copy, method)
        queue_copy = deepcopy(queue)
        expandQueue(queue_copy, method)
        closed_copy = deepcopy(closed)
        findSolution(front_copy, queue_copy, closed_copy, method)


def menu() -> Tuple[int, int, int, int, str]:
    algorithms = {'1': 'DFS', '2': 'BFS', '3': 'BestFS'}
    print('----------Pacman Abstract Menu----------')
    rows = int(input('Number of rows: '))
    columns = int(input('Number of columns: '))
    while rows < 1 or columns < 1 or (rows * columns) < 2:
        print('The maze must have at least 2 cells')
        rows = int(input('Number of rows: '))
        columns = int(input('Number of columns: '))
    try:
        num_of_fruits = int(input('Number of fruits(press enter for random number): '))
    except ValueError:
        num_of_fruits = -1
    try:
        num_of_walls = int(input('Number of walls(press enter for random number): '))
    except ValueError:
        num_of_walls = -1

    print('Choose the search algorithm to be used:')
    print('1. DFS')
    print('2. BFS')
    print('3. Best-First')
    # if the input is not in algorithms return the default value DFS
    return rows, columns, num_of_fruits, num_of_walls, algorithms.get(input('Choice 1-3: '), 'DFS')


def main():
    rows, columns, num_of_fruits, num_of_walls, method = menu()
    
    initial_state = State(rows, columns, num_of_fruits, num_of_walls)
    print('----------Starting State----------')
    print(initial_state)
    input('press enter to continue... ')
    print('----------Searching Begins----------')
    front = [initial_state]

    start = time.process_time()
    findSolution(front, [front], method=method)
    print(f'CPU Time taken = {time.process_time() - start}')


if __name__ == '__main__':
    main()
