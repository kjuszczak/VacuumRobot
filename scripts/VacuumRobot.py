import pygame


class VacuumRobot:
    def __init__(self, pos=(0, 0), angle=0):
        self.original_image = pygame.image.load("data/robot.bmp").convert()
        self.image = self.original_image
        self.rect = self.image.get_rect()
        self.rect.center = pos
        self.angle = angle
        self.x, self.y = self.rect.center

    def move(self, x, y):
        diff_x = x - self.x
        diff_y = y - self.y
        self.x = x
        self.y = y
        self.rect = self.rect.move(diff_x, diff_y)

    def rotate(self, angle):
        self.angle = angle % 360
        self.image = pygame.transform.rotate(self.original_image, self.angle)
        x, y = self.rect.center  # Save its current center.
        self.rect = self.image.get_rect()  # Replace old rect with new rect.
        self.rect.center = (x, y)  # Put the new rect's center at old center.
        self.x, self.y = self.rect.center

    def draw(self, surface):
        surface.blit(self.image, self.rect)