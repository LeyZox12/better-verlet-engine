import pygame
from copy import deepcopy as cope
from math import hypot

pygame.init()
screen = pygame.display.set_mode((1000,1000))

class Point:
	def __init__(self, x:float, y:float, radius:float, static:bool):
		self.x = x
		self.y = y
		self.radius = radius
		self.static = static
		self.friction = 600
		self.ox = x
		self.oy = y
		self.vx = 0
		self.vy = 0

	def update(self, dt:float):
		nx = self.x*2 -self.ox + self.vx * dt * dt
		ny = self.y*2 -self.oy + self.vy * dt * dt
		
		self.ox = cope(self.x)
		self.oy = cope(self.y)
		
		self.x,self.y = nx, ny
	
	def draw(self, screen):
		pygame.draw.circle(screen, (255,255,255),(self.x, self.y), self.radius)

class Constraint:
	def __init__(self, i1, i2, type, dist):
		self.i1 = i1
		self.i2 = i2
		self.dist = dist
		self.type = type

class Rect:
	def __init__(self, x, y, w, h):
		self.x = x
		self.y = y
		self.w = w
		self.h = h
	def draw(self, screen):
		pygame.draw.rect(screen, (255, 255, 255), (self.x, self.y, self.w, self.h))

class PointEngine:
	def __init__(self):
		self.points = []
		self.rects = []
		self.constraints = []
		self.substeps = 4
	def addPoint(self, x, y, radius , static):
		self.points.append(Point(x, y, radius, static))
	def link(self, i1, i2, type, dist):
		if dist == 0:
			p1 = self.points[i1]
			p2 = self.points[i2]
			dist = hypot(p1.x - p2.x, p1.y - p2.y)
		self.constraints.append(Constraint(i1,i2,type,dist))
	def addRect(self, x, y, w, h):
		self.rects.append(Rect(x, y, w, h))
	def applyCollisions(self):
		for _ in range(self.substeps):
			for i, p in enumerate(self.points):
				for j, p2 in enumerate(self.points):
					if j!=i:
						dx = p.x - p2.x
						dy = p.y - p2.y
						dist = hypot(dx,dy)
						rads = p.radius+p2.radius-dist
						if dist<p.radius + p2.radius and dist > 0:
							dx/=dist
							dy/=dist
							
							r1 = p.radius/p2.radius
							r2 = p2.radius/p.radius
							
							m1=1-r2 if r2 < r1 else r1
							m2 = 1-r1 if r1<r2 else r2
							
							p.x += dx * rads * m2
							p.y += dy * rads * m2
							p2.x -= dx*rads * m1
							p2.y -= dy * rads * m1
							#p.ox = cope(p.x)
							#p.oy = cope(p.y)
							#p2.ox = cope(p2.x)
							#p2.oy = cope(p2.y)
							
				for r in self.rects:
					index = 0
					if p.y+p.radius>r.y and p.y-p.radius<r.y+r.h and p.x+p.radius>r.x and p.x-p.radius<r.x+r.w:
						distances = [abs(p.y + p.radius - r.y), abs(p.y-p.radius-(r.y+r.h)),abs(p.x+p.radius-r.x),abs(p.x-p.radius-(r.x-r.w))]
						index = min(range(len(distances)))+1
						if index ==1:
							p.vx =(p.ox - p.x)* p.friction
							p.y = r.y -p.radius
							p.oy = r.y - p.radius
						elif index == 2:
							p.y = r.y+r.h+p.radius
							p.oy = r.y+r.h+p.radius
						elif index == 3:
							p.x = r.x-p.radius
							p.ox = r.x-p.radius
						elif index == 4:
							p.x = r.x+r.w+p.radius
							p.ox = r.x+r.w+p.radius
	def applyConstraints(self):
		for _ in range(self.substeps):
			for c in self.constraints:
				p1 = self.points[c.i1]
				p2 = self.points[c.i2]
				dx = p1.x-p2.x
				dy = p1.y-p2.y
				dist = hypot(dx, dy)
				diff = c.dist - dist
				if c.type == 0:
					dx /= dist
					dy /= dist
					m1 = 1 if p2.static else 0.5 if not p1.static else 0
					m2 = 1 if p1.static else 0.5 if not p2.static else 0
					#m1,m2 = 0.5, 0.5
					p1.x += diff * dx * m1
					p1.y += diff * dy * m1
					p2.x -= diff * dx * m2
					p2.y -= diff * dy * m2
				
	def update(self, dt):
		for p in self.points:
			if not p.static:
				p.vy+=9.8
				p.update(dt)
				p.vy*=0.98
		self.applyCollisions()
		self.applyConstraints()
	def display(self, screen):
		for p in self.points:
			p.draw(screen)
		for r in self.rects:
			r.draw(screen)
			

clock = pygame.time.Clock()
pe = PointEngine()
pe.addPoint(200,100,20,False)
pe.addPoint(270, 150, 30, False)
for i in range(30):
	pe.addPoint(250+i*9, 100, 4.5,  True if i ==100 or i == 39 else False)
	if i > 0:
		pe.link(len(pe.points)-2, len(pe.points)-1, 0, 0)
pe.addRect(0,1400,1000,1000)

while True:
	dt= clock.tick()			
	screen.fill((0,0,0))	
	pe.update(dt/1000)
	pe.display(screen)
	pygame.display.flip()