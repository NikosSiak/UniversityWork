import random
from copy import deepcopy

class State:
    ''' 
    Class that represents the world of pacman.

    Attributes:

    pacman_location: A list containing the location of pacman in the world as follows `row, column`\n
    walls_locations: A list with the locations of all the walls in the world as tuples `(row, column)`\n
    fruits_locations: A list with the locations of all fruits in the world as tuples `(row, column)`\n
    '''

    # in python methods and variables that begin with __ are private
    # class "constants" to index rows and cols
    __ROW_INDEX = 0
    __COL_INDEX = 1

    def __init__(self, rows: int, columns: int, num_of_fruits: int = -1, num_of_walls: int = -1):
        '''
        args

        rows: int
            number of rows.

        columns: int
            number of columns.

        num_of_fruits: int number of fruits to put in the world. if left as default a random number 
        between 1 and (rows*columns)//2) of fruits will be generated.

        num_of_walls: int number of walls to put in the world. if left as default a random number
        between 0 and (rows*columns)//3) of walls will be generated.
        '''

        # initialize variables
        self.__rows = rows
        self.__columns = columns
        self.pacman_location = []
        self.walls_locations = []
        self.fruits_locations = []

        # put pacman in a random position in the world
        self.pacman_location = [random.randint(0, rows-1), random.randint(0, columns-1)]

        # if num_of_walls is negative or the number exceeds the number of empty cells pick a random number
        if num_of_walls < 0 or num_of_walls > (rows * columns) - 1:
            num_of_walls = random.randint(0, (rows*columns)//3)
        # _ is a python variable name that indicates we don't care about the value
        for _ in range(num_of_walls):
            wall_row = random.randint(0, rows-1)
            wall_col = random.randint(0, columns-1)
            # find new random values until it's an empty cell
            while (wall_row, wall_col) in self.walls_locations or [wall_row, wall_col] == self.pacman_location:
                wall_row = random.randint(0, rows-1)
                wall_col = random.randint(0, columns-1)
            # append the location of the wall as a tuple. We append it as a tuple because it can't change position
            self.walls_locations.append((wall_row, wall_col))

        # if num_of_fruits is negative or the number exceeds the number of empty cells pick a random number
        if num_of_fruits < 0 or num_of_fruits > (rows * columns) - num_of_walls:
            num_of_fruits = random.randint(1, (rows*columns)//2)
        for _ in range(num_of_fruits):
            fruit_row = random.randint(0, rows-1)
            fruit_col = random.randint(0, columns-1)
            # find new random values until it's an empty cell or a cell with pacman inside
            while (fruit_row, fruit_col) in self.fruits_locations or (fruit_row, fruit_col) in self.walls_locations:
                fruit_row = random.randint(0, rows-1)
                fruit_col = random.randint(0, columns-1)
            # append the location of the fruit as a tuple
            self.fruits_locations.append((fruit_row, fruit_col))

    def __eq__(self, other) -> bool:
        ''' 
        overload ==
        '''

        if self.__rows != other.__rows:
            return False
        if self.__columns != other.__columns:
            return False
        if self.pacman_location != other.pacman_location:
            return False
        if self.fruits_locations != other.fruits_locations:
            return False
        if self.walls_locations != other.walls_locations:
            return False
        return True

    def __str__(self) -> str:
        '''
        overload str. __str__ is called when you print the object of the class. w indicates a wall,
        f indicates a fruit, p indicates the pacman and blank are the passageways
        
        print the world in the format:
        +---+---+---+
        | f | w |   |
        +---+---+---+
        |   | f | w |
        +---+---+---+
        | w |p f|   |
        +---+---+---+
        '''

        res = ''
        for i in range(self.__rows):
            res += '+---' * self.__columns + '+\n'
            for j in range(self.__columns):
                if (i, j) in self.walls_locations:
                    res += '| w '
                elif (i, j) in self.fruits_locations and [i, j] == self.pacman_location:
                    res += '|p f'
                elif (i, j) in self.fruits_locations:
                    res += '| f '
                elif (i, j) == tuple(self.pacman_location):
                    res += '| p '
                else:
                    res += '|   '
            res += '|\n'
        
        res += '+---' * self.__columns + '+\n'
        return res

    # in python methods and variables that begin with _ are by convention protected
    def _canEat(self) -> bool:
        '''
        Returns True if pacman is in a cell with a fruit
        '''

        # convert pacman_location to tuple because fruits_locations stores the locations as tuples
        return tuple(self.pacman_location) in self.fruits_locations

    def _canMoveLeft(self) -> bool:
        '''
        Returns True if pacman can go left. Pacman can't go left if he is at the very left of the world or if
        there is a wall to his left
        '''

        # with \ you can split instruction in 2 lines
        # locations are between 0-__rows and 0-__cols
        return self.pacman_location[self.__COL_INDEX] > 0 \
            and (self.pacman_location[self.__ROW_INDEX], self.pacman_location[self.__COL_INDEX]-1) not in self.walls_locations

    def _canMoveRight(self) -> bool:
        '''
        Returns True if pacman can go right. Pacman can't go right if he is at the very right of the world or if
        there is a wall to his right
        '''

        # locations are up to self.__columns-1, because indexing starts at 0
        return self.pacman_location[self.__COL_INDEX] < self.__columns-1 \
            and (self.pacman_location[self.__ROW_INDEX], self.pacman_location[self.__COL_INDEX]+1) not in self.walls_locations

    def _canMoveUp(self) -> bool:
        '''
        Returns True if pacman can go up. Pacman can't go up if he is at the top of the world or if
        there is a wall right above him
        '''

        return self.pacman_location[self.__ROW_INDEX] > 0 \
            and (self.pacman_location[self.__ROW_INDEX]-1, self.pacman_location[self.__COL_INDEX]) not in self.walls_locations

    def _canMoveDown(self) -> bool:
        '''
        Returns True if pacman can go down. Pacman can't go down if he is at the bottom of the world or if
        there is a wall right below him
        '''

        return self.pacman_location[self.__ROW_INDEX] < self.__rows-1 \
            and (self.pacman_location[self.__ROW_INDEX]+1, self.pacman_location[self.__COL_INDEX]) not in self.walls_locations

    def eat(self):
        '''
        If pacman is in the same cell as a fruit, it removes the fruit from the cell
        '''

        # remove from the list the location of the fruit that is in the same location as the pacman
        # index returns the index of the location of the pacman in the fruits_locations list
        # and because pacman is in the same location as a fruit we remove that position from the fruits_locations
        if self._canEat():
            self.fruits_locations.pop(self.fruits_locations.index(tuple(self.pacman_location)))

    def moveLeft(self):
        '''
        If pacman is able to move to the left, it moves it to the left
        '''

        if self._canMoveLeft():
            self.pacman_location[self.__COL_INDEX] = self.pacman_location[self.__COL_INDEX]-1
        
    def moveRight(self):
        '''
        If pacman is able to move to the right, it moves it to the right
        '''

        if self._canMoveRight():
            self.pacman_location[self.__COL_INDEX] = self.pacman_location[self.__COL_INDEX]+1

    def moveUp(self):
        '''
        If pacman is able to move up, it moves it up
        '''

        if self._canMoveUp():
            self.pacman_location[self.__ROW_INDEX] = self.pacman_location[self.__ROW_INDEX]-1

    def moveDown(self):
        '''
        If pacman is able to move down, it moves it down
        '''

        if self._canMoveDown():
            self.pacman_location[self.__ROW_INDEX] = self.pacman_location[self.__ROW_INDEX]+1

    def reachedGoal(self) -> bool:
        '''
        Return True if there are no fruits in the world
        '''

        return self.fruits_locations == []

    def distanceToClosestFruit(self) -> int:
        '''
        Returns the number of steps to the closest fruit
        '''
        
        if self.fruits_locations == []:
            return -1
        
        # find all distance from the location of pacman to each fruit and return the min value
        # distance is counted abs(fruit[row]-pacman[row]) + abs(fruit[col]-pacman[col])
        # ignnoring all walls for the moment

        # distances = []
        # for fruit in self.fruits_locations:
        #     distance = abs(fruit[self.__ROW_INDEX]-self.pacman_location[self.__ROW_INDEX]) + abs(
        #         fruit[self.__COL_INDEX]-self.pacman_location[self.__COL_INDEX])
        #     distances.append(distance)
        # return min(distances)
        return min([abs(fruit[self.__ROW_INDEX] - self.pacman_location[self.__ROW_INDEX]) + 
            abs(fruit[self.__COL_INDEX] - self.pacman_location[self.__COL_INDEX]) for fruit in self.fruits_locations])

    def __deepcopy__(self, memo):
        '''
        Override deepcopy
        '''

        cls = self.__class__
        result = cls.__new__(cls)
        memo[id(self)] = result
        for k, v in self.__dict__.items():
            setattr(result, k, deepcopy(v, memo))
        return result
