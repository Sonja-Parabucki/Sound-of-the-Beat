#version 330 core

//Kanali (in, out)
layout(location = 0) in vec2 inPos; //Pozicija temena
layout(location = 1) in vec4 inCol; //Boja temena - ovo saljemo u fragment sejder
out vec4 chCol; //Izlazni kanal kroz koji saljemo boju do fragment sejdera

uniform vec2 uR;
uniform float uAspect;     // Aspect ratio

void main()
{
	gl_Position = vec4(inPos.x * uAspect + uR[0], inPos.y + uR[1], 0.0, 1.0); //gl_Position je predefinisana promenljiva za pozicije u koju stavljamo nase koordinate. Definisana je kao vec4 pa zbog toga konvertujemo
	chCol = inCol;
}