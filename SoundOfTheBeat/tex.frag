#version 330 core

in vec2 chTex; //koordinate teksture
out vec4 outCol;

uniform sampler2D uTex; //teksturna jedinica

void main()
{
	outCol = texture(uTex, chTex) * 0.4; //boja na koordinatama chTex teksture vezane na teksturnoj jedinici uTex

	//mesanje 2 teksture se moze raditi sa mix(T1, T2, k) funkcijom gde su
		//T1 i T2 pozivi texture funkcije, a k koeficijent jacine druge teksture od 0 do 1 (0.3 = 70%T1 + 30%T2)
	//mesanje sa bojom temena se moze odraditi mnozenjem vektora boja i tekstura
}