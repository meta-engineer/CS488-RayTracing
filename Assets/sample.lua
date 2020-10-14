-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25)
grass = gr.material({0.1, 0.3, 0.1}, {0.2, 0.2, 0.2}, 0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25)
reflect = gr.material({0.6, 0.6, 7}, {0.5, 0.4, 0.8}, 90)

scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(0, -2, -18)


-- the floor
plane = gr.mesh( 'plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:scale(30, 30, 30)

-- "sphere"
poly = gr.mesh( 'poly', 'dodeca.obj' )
scene:add_child(poly)
poly:translate(9, 1.618034, -4)
poly:set_material(blue)

-- beeg mirror
mr = gr.cube('mirror')
scene:add_child(mr)
mr:set_material(reflect)
mr:scale(5,8,5)
mr:rotate('y', 45)
mr:translate(-4,0, -6)

p1 = gr.cube('p1')
scene:add_child(p1)
p1:set_material(reflect)
p1:scale(2,10,2)
p1:translate(-5,0, 5)
p2 = gr.cube('p2')
scene:add_child(p2)
p2:set_material(reflect)
p2:scale(2,10,2)
p2:translate(3,0, 5)

c = gr.mesh( 'poly', 'cylinder.obj' )
scene:add_child(c)
c:scale(1,2,1)
c:translate(-7, 0, 15)
c:set_material(blue)

c2 = gr.mesh('cow2', 'cow.obj')
scene:add_child(c2)
c2:rotate('y', 90)
c2:translate(7, 3.637, 20)
c2:set_material(gold)

cow = gr.mesh('cow', 'cow.obj' )
scene:add_child(cow)
cow:translate(-20, 3.637, -5.5)
cow:set_material(gold)

-- The lights
l1 = gr.light({30, 30, 30}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({4, 0, 5}, {0.5, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'sample.png', 512, 512, 
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})
