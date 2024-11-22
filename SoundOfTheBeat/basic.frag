#version 330 core

//Kanali (in, out)
in vec4 chCol; //Kanal iz Verteks sejdera - mora biti ISTOG IMENA I TIPA kao u vertex sejderu
out vec4 outCol; //Izlazni kanal koji ce biti zavrsna boja temena

void main()
{
	outCol = chCol;
}