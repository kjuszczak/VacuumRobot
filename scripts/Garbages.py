import pygame


class PieceOfGarbage:
    def __init__(self, pos=(0, 0)):
        self.image = pygame.image.load("data/garbage.bmp").convert()
        self.rect = self.image.get_rect()
        self.rect.center = pos
        self.x, self.y = pos

    def draw(self, surface):
        surface.blit(self.image, self.rect)


class Garbage:
    def __init__(self, pieces=None):
        self.pieces = []
        if pieces is not None:
            self.pieces = pieces

    def addPiece(self, piece):
        self.pieces.append(piece)

    def deletePiece(self, x, y):
        for piece in self.pieces:
            if piece.x == x and piece.y == y:
                self.pieces.remove(piece)

    def draw(self, surface):
        for piece in self.pieces:
            piece.draw(surface)