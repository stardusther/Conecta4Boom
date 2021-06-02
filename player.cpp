#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include "player.h"
#include "environment.h"

using namespace std;

const double masinf=9999999999.0, menosinf=-9999999999.0;


// Constructor
Player::Player(int jug){
    jugador_=jug;
}

// Actualiza el estado del juego para el jugador
void Player::Perceive(const Environment & env){
    actual_=env;
}

double Puntuacion(int jugador, const Environment &estado){
    double suma=0;

    for (int i=0; i<7; i++)
      for (int j=0; j<7; j++){
         if (estado.See_Casilla(i,j)==jugador){
            if (j<3)
               suma += j;
            else
               suma += (6-j);
         }
      }

    return suma;
}


// Funcion de valoracion para testear Poda Alfabeta
double ValoracionTest(const Environment &estado, int jugador){
    int ganador = estado.RevisarTablero();

    if (ganador==jugador)
       return 99999999.0; // Gana el jugador que pide la valoracion
    else if (ganador!=0)
            return -99999999.0; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
            return 0;  // Hay un empate global y se ha rellenado completamente el tablero
    else
          return Puntuacion(jugador,estado);
}

// ------------------- Los tres metodos anteriores no se pueden modificar




// Devuelve un valor segun la heuristica de cada casilla seleccionada
double ValorCasilla(const Environment &estado,int jugador,int fila,int col){

    double valor = 0;

    //Casilla seleccionada
    int casillaSelec = estado.See_Casilla(fila,col);
    //Casillas que están al lado de la casilla a valoruar
    int casillaCercana;


    //Ficha bomba j1 pertenece al j1
    if (casillaSelec==4)
        casillaSelec=1;

    //Ficha bomba j2 pertenece al j2
    else if(casillaSelec==5)
        casillaSelec == 2;

    //Ficha vacia = 0 , pertenece al jugar actual
    else if(casillaSelec == 0)
        casillaSelec == jugador;


    //Analizamos las 8 casillas que rodean a la seleccionada
    for (int i=fila-1; i<=fila+1; i++){
        for (int j=col-1; j<=col+1; j++){

            //Si la casilla seleccionada es la correcta y esta dentro del tablero
            if(i!=fila && j!=col && i>=0 && i<7 && j>=0 && j<7){

                casillaCercana = estado.See_Casilla(i,j);

                //Si la casilla es la del jugador actual comprobamos que no este al lado de ninguna de su mismo color y de ser asi se reduce el valor
                if((casillaCercana == casillaSelec || casillaCercana == casillaSelec + 3) && casillaSelec == jugador)
                    valor = valor -2 ;

            //En caso contrario aumentamos la valoracion. Ya que estara al lado de distinto color, o de una del adversario.
            else
                valor++;
         }
      }
   }

return valor;
}


//heuristica. Devuelve la suma de todos los valores de las casillas del tablero
double func_heur(const Environment &estado,int jugador){
    double total;

    for (int fil=0; fil<7; ++fil)       //Recorrer la matriz
        for (int col=0; col<7; ++col)
            total += ValorCasilla(estado,jugador,fil,col);

    return total;
}

// Funcion heuristica (ESTA ES LA QUE TENEIS QUE MODIFICAR)
double Valoracion(const Environment &estado, int jugador){

    int ganador = estado.RevisarTablero();

    if (ganador==jugador)
        return 99999999.0; // Gana el jugador que pide la valoracion
    else if (ganador != 0)
        return -99999999.0; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
        return 0;  // Hay un empate global y se ha rellenado completamente el tablero
    else
        return func_heur(estado,jugador);
}

// Esta funcion no se puede usar en la version entregable
// Aparece aqui solo para ILUSTRAR el comportamiento del juego
// ESTO NO IMPLEMENTA NI MINIMAX, NI PODA ALFABETA
void JuegoAleatorio(bool aplicables[], int opciones[], int &j){
    j=0;
    for (int i=0; i<8; i++){
        if (aplicables[i]){
           opciones[j]=i;
           j++;
        }
    }
}


double Poda_AlfaBeta(const Environment &actual,int jugador, int profundidad,  int PROFUNDIDAD_ALFABETA, Environment::ActionType &accion, double alpha, double beta ){

    double valor_nodo;        // Valor del nodo
    Environment hijo[8];      // Nodo Inicial y todos sus estados

    int num_hijos = actual.GenerateAllMoves(hijo);
    Environment estado = hijo[0];

    // Si hemos llegado a la profundidad MAX
    if (profundidad == PROFUNDIDAD_ALFABETA || actual.JuegoTerminado())
        return Valoracion(actual,jugador);

    //Estamos en un nodo MAX si se cumple ;
    if (actual.JugadorActivo()==jugador)
    {

        for(int i = 0; i< num_hijos; i++)
        {

            valor_nodo = Poda_AlfaBeta(hijo[i], jugador, profundidad+1, PROFUNDIDAD_ALFABETA, accion, alpha, beta);

            if (valor_nodo > alpha )
            {
                alpha = valor_nodo;
                estado = hijo[i];
            }

            if (beta <= alpha) // Si beta es menor que alpha hemos terminado
                break;
        }

        accion = static_cast <Environment::ActionType>(estado.Last_Action(jugador));
        return alpha;   // Devolvemos el nodo alpha

    } else  { // Si no , estamos en un nodo MIN
        for (int i = 0 ; i < num_hijos ; i++)
        {
            valor_nodo = Poda_AlfaBeta(hijo[i], jugador, profundidad+1, PROFUNDIDAD_ALFABETA, accion, alpha, beta);

            if (valor_nodo < beta)
            {
                beta = valor_nodo;
                estado = hijo[i];
            }

            if (beta <= alpha)  // Si beta es menor que alpha hemos terminado
                break;
        }

        accion = static_cast <Environment::ActionType >(estado.Last_Action(jugador));
        return beta;
    }
}


// Invoca el siguiente movimiento del jugador
Environment::ActionType Player::Think(){
    const int PROFUNDIDAD_MINIMAX = 6;  // Umbral maximo de profundidad para el metodo MiniMax
    const int PROFUNDIDAD_ALFABETA = 8; // Umbral maximo de profundidad para la poda Alfa_Beta

    Environment::ActionType accion; // acci�n que se va a devolver
    bool aplicables[8]; // Vector bool usado para obtener las acciones que son aplicables en el estado actual. La interpretacion es
                        // aplicables[0]==true si PUT1 es aplicable
                        // aplicables[1]==true si PUT2 es aplicable
                        // aplicables[2]==true si PUT3 es aplicable
                        // aplicables[3]==true si PUT4 es aplicable
                        // aplicables[4]==true si PUT5 es aplicable
                        // aplicables[5]==true si PUT6 es aplicable
                        // aplicables[6]==true si PUT7 es aplicable
                        // aplicables[7]==true si BOOM es aplicable



    double valor=0.00;// Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha=menosinf, beta=masinf; // Cotas de la poda AlfaBeta

    int n_act; //Acciones posibles en el estado actual


    n_act = actual_.possible_actions(aplicables); // Obtengo las acciones aplicables al estado actual en "aplicables"
    int opciones[10];

    // Muestra por la consola las acciones aplicable para el jugador activo
    //actual_.PintaTablero();
    cout << " Acciones aplicables ";
    (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
    for (int t=0; t<8; t++)
      if (aplicables[t])
         cout << " " << actual_.ActionStr( static_cast< Environment::ActionType > (t)  );
    cout << endl;

/*
    //--------------------- COMENTAR Desde aqui
    cout << "\n\t";
    int n_opciones=0;
    JuegoAleatorio(aplicables, opciones, n_opciones);

    if (n_act==0){
      (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
      cout << " No puede realizar ninguna accion!!!\n";
      //accion = Environment::actIDLE;
    }
    else if (n_act==1){
           (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
            cout << " Solo se puede realizar la accion "
                 << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[0])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[0]);

         }
         else { // Hay que elegir entre varias posibles acciones
            int aleatorio = rand()%n_opciones;
            cout << " -> " << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[aleatorio])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[aleatorio]);
         }

    //--------------------- COMENTAR Hasta aqui
*/

    //--------------------- AQUI EMPIEZA LA PARTE A REALIZAR POR EL ALUMNO ------------------------------------------------

    if (actual_.See_Casilla(0,0) == 0 &&    // Para que empiece por la casilla central siempre oorque si no me da TOC
	  actual_.See_Casilla(0,1) == 0 &&
	  actual_.See_Casilla(0,2) == 0 &&
	  actual_.See_Casilla(0,3) == 0 &&
	  actual_.See_Casilla(0,4) == 0 &&
	  actual_.See_Casilla(0,5) == 0 &&
	  actual_.See_Casilla(0,6) == 0)
	{
	  cout << "ES LA PRIMERA TIRADA" << endl;
	  return Environment::PUT4;

	}

    // Opcion: Poda AlfaBeta
    // NOTA: La parametrizacion es solo orientativa
    valor = Poda_AlfaBeta(actual_, jugador_, 0, PROFUNDIDAD_ALFABETA, accion, alpha, beta);
    cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;

    return accion;
}
