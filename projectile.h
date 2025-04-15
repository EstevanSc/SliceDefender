#ifndef PROJECTILE_H
#define PROJECTILE_H

class Projectile
{
public:
    Projectile(int finalPosition[3], int startPosition[3], float flyingTime); // constructor to initialize the projectile with final position, start position and flying time
    virtual void draw();                                                      // abstract fonction draw() to be implemented in derived classes
    virtual void update();                                                    // abstract fonction update() to be implemented in derived classes
    void move();                                                              // function to move the projectile
    virtual int *getPosition() const;                                         // function to get the current position of the projectile
    virtual void setPosition(float x, float y, float z);                      // function to set the position of the projectile

private:
    static const int canonAngle = 20; // angle of the canon in degrees
    int m_finalPosition[3];           // final position of the projectile
    int m_startPosition[3];           // starting position of the projectile
    float m_flyingTime;               // flying time of the projectile
    float m_timeOfLaunch = 0.0f;      // time of launch
};

#endif // PROJECTILE_H
