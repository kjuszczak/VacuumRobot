import pygame


class RectangleParameters:
    def __init__(self, x, y, length, width):
        self.x = x
        self.y = y
        self.length = length
        self.width = width

    def draw(self, surface, color):
        pygame.draw.rect(surface, color, [self.x, self.y, self.length, self.width], 0)


class Furniture():
    def __init__(self, coordinates, color):
        self.coordinates = coordinates
        self.color = color

    def draw(self, surface):
        for coordinate in self.coordinates:
            coordinate.draw(surface, self.color)

class Furnitures():
    def __init__(self):
        self.furnitures = []

    def addFurniture(self, furniture):
        self.furnitures.append(furniture)

    def draw(self, surface):
        for furniture in self.furnitures:
            furniture.draw(surface)