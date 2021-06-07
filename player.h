#ifndef PLAYER_H
#define PLAYER_H

#include "environment.h"

class Player{
    public:
      Player(int jug);
      Environment::ActionType Think();
      void Perceive(const Environment &env);
      //double MiniMax(const Environment & tablero, int jugador, int profundidad, int profundidad_maxima, Environment::ActionType & accion);
      double Poda_AlfaBeta(const Environment & actual, int jugador, int profundidad, int profundidad_maxima, Environment::ActionType & accion, double alfa, double beta);
    private:
      int jugador_;
      Environment actual_;
};
#endif
