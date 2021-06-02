#ifndef PLAYER_H
#define PLAYER_H

#include "environment.h"

class Player{
    public:
      Player(int jug);
      Environment::ActionType Think();
      void Perceive(const Environment &env);
      //double MiniMax(const Environment & tablero, int jugador, int profundidad, int profundidad_maxima, Environment::ActionType & accion);
      double AlfaBeta(const Environment & tablero, int jugador, int profundidad, int profundidad_maxima, Environment::ActionType & accion, double alfa, double beta);
double NegaMax(const Environment & tablero, int jugador, int profundidad, int profundidad_maxima, Environment::ActionType & accion);
    private:
      int jugador_;
      Environment actual_;
};
#endif
