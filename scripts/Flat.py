import pygame
from Garbages import Garbage, PieceOfGarbage
from Walls import Walls, Wall, Door
from Furnitures import Furnitures, Furniture, RectangleParameters
from VacuumRobot import VacuumRobot

# Initialize values for color (RGB format)
WHITE = (255, 255, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
BLACK = (0, 0, 0)
BROWN = (160, 82, 45)


class Flat:
    def __init__(self, surface_length, surface_width):
        self.drawFlag = False
        self.surface_length = surface_length
        self.surface_width = surface_width
        self.flat_x = 0
        self.flat_y = 0
        self.length = 0
        self.width = 0
        self.line_width = 3
        self.color = BLACK
        self.x = 0
        self.y = 0
        self.angle = 0
        self.walls = Walls()
        self.robot = VacuumRobot((self.x, self.y), self.angle)
        self.furnitures = Furnitures()
        self.garbages = Garbage()

    def createFlat(self, json_paramaters):
        self.length = int(json_paramaters['length']/2)
        self.width = int(json_paramaters['width']/2)
        self.flat_x = int((self.surface_length - self.length) / 2)
        self.flat_y = int((self.surface_width - self.width) / 2)
        self.createWalls(json_paramaters)
        self.createDoors(json_paramaters)
        self.createFurnitures(json_paramaters)
        self.createGarbage(json_paramaters)
        self.drawFlag = True

    def createWalls(self, json_parameters):
        walls = json_parameters['walls']
        for wall in walls:
            start_pos = (self.flat_x + int(wall['start_x'] / 2), self.flat_y + int(wall['start_y'] / 2))
            end_pos = (self.flat_x + int(wall['end_x'] / 2), self.flat_y + int(wall['end_y'] / 2))
            self.walls.addWall(Wall([start_pos, end_pos], BLACK, 3))

    def createDoors(self, json_parameters):
        doors = json_parameters['doors']
        for door in doors:
            start_pos = (self.flat_x + int(door['start_x'] / 2), self.flat_y + int(door['start_y'] / 2))
            end_pos = (self.flat_x + int(door['end_x'] / 2), self.flat_y + int(door['end_y'] / 2))
            self.walls.addDoor(Door([start_pos, end_pos], WHITE, 3))

    def createFurnitures(self, json_parameters):
        furnitures = json_parameters['furnitures']
        for furniture in furnitures:
            rectangle_object = []
            rec_json = furniture['furniture']
            for rec in rec_json:
                x = self.flat_x + int(rec['x'] / 2)
                y = self.flat_y + int(rec['y'] / 2)
                length = int(rec['length'] / 2)
                width = int(rec['width'] / 2)
                rectangle_object.append(RectangleParameters(x, y, length, width))
            self.furnitures.addFurniture(Furniture(rectangle_object, BROWN))

    def createGarbage(self, json_parameters):
        garbages = json_parameters['garbages']
        for garbage in garbages:
            x = self.flat_x + int(garbage['x'] / 2)
            y = self.flat_y + int(garbage['y'] / 2)
            self.addGarbage(x, y)

    def addGarbage(self, x, y):
        self.garbages.addPiece(PieceOfGarbage((x, y)))

    def deleteGarbage(self, x, y):
        self.garbages.deletePiece(x, y)

    def updateRobotCoordinates(self, x, y, angle):
        self.x = self.flat_x + int(x/2)
        self.y = self.flat_y + int(y/2)
        self.angle = angle
        self.robot.move(self.x, self.y)
        self.robot.rotate(self.angle)

    def updateParameters(self, json_parameters):
        x = json_parameters['x']
        y = json_parameters['y']
        angle = json_parameters['angle']
        self.updateRobotCoordinates(x, y, angle)
        isAddGarbages = json_parameters['add garbages']
        if isAddGarbages:
            self.createGarbage(json_parameters)
        isDeleteGarbages = json_parameters['delete garbages']
        if isDeleteGarbages:
            array_of_garbages = json_parameters['garbages']
            for garbage in array_of_garbages:
                x = int(garbage['x'] / 2)
                y = int(garbage['y'] / 2)
                self.deleteGarbage(x, y)

    def drawFlat(self, surface):
        surface.fill(WHITE)
        pygame.draw.rect(surface, self.color, [self.flat_x, self.flat_y, self.length, self.width], self.line_width)

    def draw(self, surface):
        if not self.drawFlag:
            return
        self.drawFlat(surface)
        self.walls.draw(surface)
        self.furnitures.draw(surface)
        self.garbages.draw(surface)
        self.robot.draw(surface)