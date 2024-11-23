#include "menu.h"

//klik na SPACE => pocinje igru
//klik na [L]eave => izlaz iz igre

Game menu(GLFWwindow* window, unsigned int shader) {

    float vertices[] =
    {  //X    Y       R    G    B    A
       -0.06,  0.1,    1.0, 1.0, 1.0, 1.0,
        0.06,  0.0,    1.0, 1.0, 1.0, 1.0,
       -0.06,  -0.1,    1.0, 1.0, 1.0, 1.0
    };
    unsigned int stride = (2 + 4) * sizeof(float); //Korak pri kretanju po podacima o temenima, velicina jednog temena

    unsigned int VAO; //Vertex Array Object - Sadrzi bar 16 pokazivaca na atribute koji opusuju sadrzaje bafer objekata
    glGenVertexArrays(1, &VAO); //Generisi 1 VAO na adresi datoj adresi
    glBindVertexArray(VAO); //Povezi VAO za aktivni kontekst - sva naknadna podesavanja ce se odnositi na taj VAO


    unsigned int VBO; //Vertex Buffer Object - Bafer objekat, u ovom slucaju za temena trougla koji crtamo
    glGenBuffers(1, &VBO); //Generisi 1 bafer na datoj adresi 
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //Povezi dati bafer za aktivni kontekst. Array buffer se koristi za temena figura.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Slanje podataka na memoriju graficke karte
    //glBufferData(koji bafer, koliko podataka ima, adresa podataka, nacin upotrebe podataka (GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW;)

    //Opisivanje pokazivaca na atribute: Pokazivac 0 ceo opisati poziciju (koordinate x i y), a pokazivac 1 boju (komponente r, g, b i a).
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    //glVertexAttribPointer(indeks pokazivaca, broj komponenti atributa, tip komponenti atributa, da li je potrebno normalizovati podatke (nama uvek GL_FALSE), korak/velicina temena, pomeraj sa pocetka jednog temena do komponente za ovaj atribut - mora biti (void*))  
    glEnableVertexAttribArray(0); //Aktiviraj taj pokazivac i tako intepretiraj podatke
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float))); //Objasni da su preostala cetiri broja boja (preskacemo preko XY od pozicije, pa je pomeraj 2 * sizeof(float))
    glEnableVertexAttribArray(1);



    //render petlja
    glClearColor(0.0, 0.0, 0.1, 1.0);

    int next = -1;
    int mode = 0;
    while (next == -1) {
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            next = 0;
            //glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            next = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            mode = 0;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            mode = 1;
        }
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);   //_TRIANGLE_STRIP spaja 3. na prethodna 2
                                            //_TRIANGLE_FAN lepeza, zajednicko prvo teme
                                            //_LINES po dva temena spaja (glLineWidth(5);), _LINE_STRIP, _LOOP
                                            //POINTS (glPointSize(5);)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glBindVertexArray(0);
    glUseProgram(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    return Game{mode, next};
}