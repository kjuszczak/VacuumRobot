import pygame

class LineObject:
    def __init__(self, coordinates, color, width):
        self.coordinates = coordinates
        self.color = color
        self.width = width

    def draw(self, surface):
        startPos = self.coordinates[0]
        endPos = self.coordinates[1]
        pygame.draw.line(surface, self.color, startPos, endPos, self.width)


class Wall(LineObject):
    def __init__(self, coordinates, color, width):
        super().__init__(coordinates, color, width)


class Door(LineObject):
    def __init__(self, coordinates, color, width):
        super().__init__(coordinates, color, width)


class Walls:
    def __init__(self, walls=None, doors=None):
        self.walls = []
        self.doors = []
        if walls is not None:
            self.walls = walls
        if doors is not None:
            self.doors = doors

    def addWall(self, wall):
        self.walls.append(wall)

    def addDoor(self, door):
        self.doors.append(door)

    def draw(self, surface):
        for wall in self.walls:
            wall.draw(surface)
        for door in self.doors:
            door.draw(surface)