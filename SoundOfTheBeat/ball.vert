#version 330 core

//Kanali (in, out)
layout(location = 0) in vec2 inPos; //Pozicija temena
layout(location = 1) in vec4 inCol; //Boja temena - ovo saljemo u fragment sejder
out vec4 chCol; //Izlazni kanal kroz koji saljemo boju do fragment sejdera

uniform vec2 uR;
uniform float uAspect;     // Aspect ratio
uniform float uInflation;

void main()
{
	//gl_Position je predefinisana promenljiva za pozicije u koju stavljamo nase koordinate. Definisana je kao vec4 pa zbog toga konvertujemo
	gl_Position = vec4((inPos.x * uAspect * uInflation + uR[0]),
						(inPos.y * uInflation + uR[1]),
						0.0,
						1.0); 
	chCol = inCol;
}