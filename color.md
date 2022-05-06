Separate repulsion and influence
Start reading the camera then switch to video file




# Color use

* Assigned color from video pixel at their start position
* Assigned palette color from Perlin noise at their start position
* Config: bool "Color from video" or "Color from palette"
* Config: bool "Fade with age" or "Fade with distance"


************* there's more to this: pick up colour at start or during movement?



## Trails

* Alpha from age or distance
* From video: use video color
* Not "From video" use the palette colour

## Connections

* Alpha from age or distance
* From video: lerp between two connected particles
* Not from video: lerp between connected particles
