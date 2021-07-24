This badge is designed to highlight digital fabrication techniques that are commonly
used in the [Neurotechnology Core](https://www.ntcore.org/) at the
[Center for Brain Science](http://cbs.fas.harvard.edu/) at Harvard University.

The NTCore works to accelerate and advance scientific progress through:
- design and fabrication of novel instrumentation
- user training and access to a shared fabrication space
- consultation on instrumentation use, design, fabrication and purchase

By building a badge users will learn about:
- electronics design including making a custom printed circuit board (PCB) using the free and open source [KiCad](https://www.kicad.org/) design software
- electronics fabrication including soldering of surface mount electronics
- embedded programming using the [Arduino](https://www.arduino.cc/) environment
- designing mechanical parts using [Autodesk Fusion360](https://www.autodesk.com/products/fusion-360/) (free to academics)
- digital fabrication using 3D printing and laser cutting for rapid prototyping and production

Your badge comes in 4 major parts:
- Electronics
- Battery (CR2032)
- 3D printed case
- Laser cut case back cover

Click the image below for a short (~1 min) video showing the assembly process.

[![Assembly Video Link](https://img.youtube.com/vi/ZrBIpawWA10/0.jpg)](https://www.youtube.com/watch?v=ZrBIpawWA10)

First insert the battery into the electronics (positive side out).

Next, slide the electronics into the case from the back making sure that the electronics slide behind the 6 plastic tabs.

Finally, slide the laser cut case back cover in place to lock the electronics into the case. Optionally, tie a lanyard through the hole to wear the badge as a pendant.

To activate the badge press your finger against the two neurons at the bottom of the badge.

TODO add break here to jump to advanced assembly instructions

# Building your own badge

Badge assembly involves 5 steps:
- soldering electronic components to the badge printed circuit board
- programming the microcontroller
- 3D printing the main case body
- laser cutting a custom case back cover

## Soldering

All the electronic components are surface mount devices (SMD) which means they
sit on the top or bottom of the badge and do not have pins that go through the
board (like through-hole parts). Soldering SMD electronics takes some practice
and a few tips can make the process much easier. One useful technique is to solder
1 pin at the corner of a part. This pin can be reheated and the part position
adjusted with forceps while the solder is molten. When the part is correctly
aligned the opposite pin can be soldered to lock the part in place before
connecting the remaining pins.

One useful technique is to
perform the following when placing a part:
- note the polarity of the part, all packages will have some reference designating part orientation (like a small dot near pin 1)
- note the polarity of the footprint, footprints often have a small dot or line indicating pin 1
- apply heat and solder to one of the footprint pads, this will be your first anchor point
- pick up the part with tweezers, orient it near the footprint and confirm the part orientation matches the footprint
- re-heat the anchor point and place your part so that the appropriate pin attaches to the anchor point
- if the part orientation needs to be adjusted (if the other pins don't line up with their pads), reheat the anchor point while holding and then moving the part. While the solder is molten, the part can easily be moved but be careful to not move the part when the soldering is cooling or cold.
- when the part orientation is good enough, solder the pin opposite the anchor point
- re-confirm part orientation and if all is good, solder the remaining pins to their pads

TODO: images and/or video of above steps

It's often helpful to start with the shortest parts and solder the largest parts
(connectors and in this case the battery tab) last. For this badge you might want to solder parts in the following order:
- ATTiny (U1)
- Resistors (R1-R4)
- Capacitor (C1)
- LEDs (D1-3)
- battery tab (on back)

TODO add pictures of each part, describe designators and reference marks and show placement on footprint.

The ATTiny (U1) has several (8) closely spaced pins. Although the final result
must have no short circuits because of solder between these pins it's often
easier to drag solder the pins and then remove shorts. Drag soldering involves
adding a larger than needed amount of solder while heating up the first pin
on one side of the chip and then dragging this solder blob down the side of
the chip bringing solder into contact with all pins. The surface tension of
molten solder will often wick just enough solder into each pad and when you're
lucky you will have no shorts. If you see solder between pins, clean your iron
by wiping it on brass wool or a wet sponge and heat up the short with the
clean iron. This will often pull solder away from your pins and onto the iron
leaving you with short-free pins. If after a few tries you still have a short
you can try using solder wick, a fine copper mesh, to clean up the joint.
Unspool a length of solder wick, and press the wick against the joint with the
hot soldering iron. The wick will get hot and pull the solder away from the
joint.

TODO gif of drag soldering, cleaning a shorted joint

## Programming
## 3D printing
## Laser cutting


# Electronics
![pcb](/media/imgs/cbs_badge_render.png?raw=true "Rough Render")
## PCB
Although it is possible to fabricate your own printed circuit boards, the results are often less precise and more
time consuming than ordering a custom board. The cost of the board will depend on the difficulty in fabricating the design,
the lead time for the order and the quantity ordered. Designs that have traces (wires embedded in the PCB) that
are less than 6 mils (thousandths of an inch) or have many layers (pcbs are made by stacking layers of fiberglass, patterned
copper and other materials) are typically manufacturable at an increased cost or lead time. The design for this
badge is relatively simple and can be made by most low cost venders ('fab houses') including [OSHPark](https://oshpark.com/).
OSHPark is highly recommended as they produce high quality boards with a relatively short lead time (typically a week)
at a USA location allowing for overnight shipping. For their 'Prototype' service they charge $5 per square inch for a 2
layer PCB. The badge is 2.25 square inches costing $11.25 per board. When ordered with their 'Medium Run' bulk service
(which requires ordering at least 100 square inches) the cost drops to $2.25 per board.

## PCB and circuit design

![schematic](/media/imgs/schematic.png?raw=true "Schematic")

[KiCad](https://www.kicad.org/) is a free and open source electronics design program that runs on all modern operating systems. It's design
process involves a number of steps often starting with making a schematic of the circuit you would like to build.
This schematic typically includes several components (including resistors, capacitors, connectors, integrated
circuits etc) connected using traces (wires embedded in the final PCB design). After a schematic is drawn some
automated tests should be run (electrical rules check or ERC) to check for incorrect or missing connections. The
final schematic and components can then be exported as a 'netlist', footprints (custom copper shapes that fit
specific components) can then be associated with the netlist components and then imported into the pcb
layout editor. In the pcb layout editor footprints are placed on a layered design (typical low cost boards
have 2 copper layers 'top' and 'bottom') and traces are drawn to connect footprints based on the connectivity
defined in the netlist. Before finalizing a design the pcb layout editor can run several automated checks
(design rule checks or DRC) to verify that your design matches the capabilities of the fab house (you will need
to input these design rules into the editor). Fab houses typically accept gerber files (a special file format
that contains images of layers of the pcb and other features needed for fabrication). However, some houses
including OSHPark will directly accept KiCad designs.

![pcb](/media/imgs/pcb_layers.png?raw=true "PCB Layers")

## Programmable electronics using microcontrollers

The badge contains a microcontroller, a small computer designed to produce and sample digital
and analog signals. The microcontroller can be programmed (more on this below) to produce precisely
timed pulses, read and record analog voltages, control other electronics (lights, sensors, motors, etc),
perform complicated logic (state machines, event loops) and many other tasks. Many types of microcontrollers
are available with different amounts of processing power, number of connections and specialized hardware
(including wifi and other wireless communication). The badge circuit uses a simple and low cost ATTiny.
This ATTiny has only 8 connections (pins) and requires only a program and a connection to power and
ground to function. For the badge 3 pins are connected to light emitting diodes (LEDs) and one more pin
is connected to a custom neuron footprint used as a touch sensor.

# Software
## Embedded programming using Arduino

[Arduino](https://www.arduino.cc/) is a beginner friendly environment for writing programs for and configuring microcontrollers. There are
many official and non-official 'Arduino compatible' boards which can be programmed with the Arduino
integrated development environment (IDE). Arduino programs are written in the C++ programming language
and use functions that simplify the process of setting or reading the state of digital and analog pins
and other aspects of using a microcontroller. Development using Arduino starts with installing the IDE,
adding support for the board you'd like the use, installing any required libraries, and then writing
your program and 'uploading' it to your board. Most boards can be programmed over USB. However, the
badge uses a simpler method called 'in circuit serial programming' (ICSP) that requires a custom
programmer (such as the [Pocket AVR Programmer](https://www.sparkfun.com/products/9825) or 
[USBtinyISP](https://www.adafruit.com/product/46)).

For a detailed course on using Arduino please see the
['Arduino for Biologists' nanocourse](https://github.com/HMS-RIC/ArduinoNanocourse).

![neurongif](/media/gifs/neuron_firmware.gif?raw=true "Neuron firmware")

The badge 'firmware' (software for embedded hardware) simulates 3 neurons connected in a ring with 3
model excitatory synapses. The spike output of each neuron is mapped to a different LED which flashes
when the neuron spikes. When the touch sensor (copper neurons at the bottom of the badge) is touched, the
microcontroller wakes up from 'sleep' (a low power mode where very few features are active) and 
initializes the neural network by setting the synaptic weights to a low value. As the network runs,
random simulated currents are injected into the 3 neurons causing them to initially spike in a random
pattern. These spikes trigger time-dependent changes in the synaptic weights. The lack of inhibition
and asymmetry in the plasticity causes unbound excitation in the circuit and the network often
quickly converges to a state of continuous fast firing. The network will continue to run as long
as the touch sensor is held. When the touch sensor is released the microcontroller will reenter
'sleep' mode to extend battery life.

# Case
![front](/media/imgs/cad_front.png?raw=true "CAD front")
![back](/media/imgs/cad_back.png?raw=true "CAD back")

## Computer aided design (CAD) using Autodesk Fusion360

![CAD](/media/gifs/CAD_design.gif?raw=true "CAD design")

Although simple 2D mechanical parts can be designed in Inkscape, Adobe Illustrator or other vector
graphics programs, 3D parts and assemblies are often designed in specialized computer aided design
(CAD) software. The badge case was designed in [Autodesk Fusion360](https://www.autodesk.com/products/fusion-360/)
(free to academics) and involved a typical process where 2D sketches on reference planes are 'extruded' to form 3D shapes. Each
new 3D shape can be joined with previous shapes or be used to cut custom shaped holes out of existing
parts. This process creates a final (or multiple) 3D shapes and a recorded history of how these
parts were designed. This history can be edited and replayed to generate part revisions or to
adapt a single design to multiple applications. For the badge, 2 parts were designed: a 
case designed for easy 3D printing and a customizable back cover designed for laser cutting.

## 3D printed case

![3D printing](/media/gifs/3D_printing_slicing.gif?raw=true "3D printing")

3D printing is a relatively automated process for translating a digital design to a physical
object. Although many materials can be 3D printed with often very different processes, one of the
most common methods for 3D printing is fused deposition modeling (FDM) using a plastic filament, 
polylactic acid (PLA). FDM printing involves heating up and forcing (extruding) PLA through a
circular nozzle (0.4 mm in diameter) that is mounted to a XYZ movement system. By depositing
and fusing overlapping 2D layers, complex 3D shapes can be created. Preparing a design for FDM
printing involves:
-  exporting a solid model (in STL file format) from your CAD software
-  importing the STL into a 'slicer' program that:
    -  cuts the model into 2D layers
    - adds any necessary support material to deal with unsupported overhangs (where a higher 2D layer has no plastic underneath
to join to)
    - generates a sequence of movements (GCODE) to command the 3D printer to make the part
- loading and running the GCODE on a suitable 3D printer
- finally removing any support material and finishing your part

## Lasercut back cover

Lasercutting is a common technique for cutting 2D shapes out of flat sheets of plastic (typically
acrylic). A high powered (75 watt) CO2 laser is focused on the surface of the plastic at a position
determined by computer controlled XY movement. This focused light quickly ablates the plastic
under the beam and by moving the cut position in various paths, flat 2D parts can be produced.
For simple designs, vector graphics programs (Inkscape, Adobe Illustrator, Corel Draw) can be used to simply
draw the paths the laser should follow and for more complex designs sketches can be exported from
CAD software in a suitable format (DXF). For most 2D geometries lasercutting is significantly
faster, more accurate and produced a better finished part compared to 3D printing.
