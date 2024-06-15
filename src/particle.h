#include <raylib.h>
#include <iostream>
#include <vector>

typedef int Mat_Type;
#define t_air (Mat_Type)0
#define t_solid (Mat_Type)1
#define t_sand (Mat_Type)2

#define color_air WHITE
#define color_solid BLACK
#define color_sand YELLOW

class Particle
{
public:
    Particle();
    Particle(Mat_Type materialType) { _materialType = materialType; };
    Particle(Mat_Type materialType, Color color)
    {
        _materialType = materialType;
        _color = color;
    };
    Particle(Mat_Type materialType, Color color, Vector2 velocity)
    {
        _materialType = materialType;
        _color = color;
        _velocity = velocity;
    };
    Vector2 const getPosition() { return _position; };
    int const getX() { return _position.y; };
    int const getY() { return _position.x; };
    Color const getColor() { return _color; };
    void setColor(Color col) { _color = col; };
    Mat_Type const getType() { return _materialType; };

private:
    Vector2 _position = {0, 0};
    Vector2 _velocity = {0.0f, 0.0f};
    Color _color = BLACK;
    bool _hasBeenUpdated = false;
    Mat_Type _materialType = t_air;
};

class ParticleWorld
{
public:
    ParticleWorld(int width, int height);
    ~ParticleWorld();
    void UpdateParticles();
    Particle *ParticleAtCoord(int x, int y);
    Particle *ParticleAtCoord(Vector2 v);
    Particle *ParticleAtIndex(int idx);
    void SetParticle(int x, int y, Particle *particle); // sets particle at x y position to this particle
    void SetParticle(Vector2 v, Particle *particle);    // sets particle at x y position to this particle
    int CoordToIndex(int x, int y);
    int CoordToIndex(Vector2 v);
    Vector2 IndexToCoord(int idx);

protected:
    bool InBounds(int x, int y) {return x >= 0 && y >= 0 && x < _width && y < _height;}
    bool IsEmpty(Particle *p) { return p != nullptr && p->getType() == t_air; }
    void SwapParticles(int x1, int y1, int x2, int y2);
    void UpdateSand(int x, int y);

private:
    std::vector<std::pair<int, int>> _frameSwaps;
    std::vector<Particle *> _particles;
    int _maxParticles;
    int _width;
    int _height;
};

ParticleWorld::ParticleWorld(int width, int height)
{
    _maxParticles = width * height;
    _width = width;
    _height = height;
    for (int i = 0; i < _maxParticles; i++)
    {
        Particle *tmp = new Particle{t_air, color_air};
        _particles.push_back(tmp);
    }
}

ParticleWorld::~ParticleWorld()
{
    for (int i = 0; i < _maxParticles; i++)
    {
        delete _particles[i];
    }
}

void ParticleWorld::UpdateParticles()
{
    for (int y = _height - 1; y >= 0; y--)
    {
        for (int x = 0; x < _width; x++)
        {
            Particle *p = ParticleAtCoord(x, y);
            switch (p->getType())
            {
            case t_air:
            {
            }
            case t_sand:
            {
                UpdateSand(x, y);
            }
            case t_solid:
            {
            }
            }
        }
    }
}

Particle *ParticleWorld::ParticleAtIndex(int idx)
{
    if (idx >= 0 && idx < _maxParticles)
        return _particles[idx];
    return nullptr;
}

Particle *ParticleWorld::ParticleAtCoord(Vector2 v)
{
    return ParticleAtIndex(CoordToIndex(v));
}

Particle *ParticleWorld::ParticleAtCoord(int x, int y)
{
    return ParticleAtIndex(CoordToIndex(x, y));
}

void ParticleWorld::SetParticle(int x, int y, Particle *particle)
{
    if (!InBounds(x,y))
        return;
    Particle *tmp = _particles[CoordToIndex(x, y)];
    delete tmp;
    _particles[CoordToIndex(x, y)] = particle;
}

void ParticleWorld::SetParticle(Vector2 v, Particle *particle)
{
    SetParticle(v.x, v.y, particle);
}

int ParticleWorld::CoordToIndex(int x, int y)
{
    if (!InBounds(x,y))
        return -1;
    return y * _width + x;
}

int ParticleWorld::CoordToIndex(Vector2 v)
{
    return CoordToIndex(v.x, v.y);
}

Vector2 ParticleWorld::IndexToCoord(int idx)
{
    int x = idx % _width;
    int y = idx / _width;
    Vector2 vec{(float)x, (float)y};
    return vec;
}

void ParticleWorld::UpdateSand(int x, int y)
{
    int direction = 1;
    Particle *b = ParticleAtCoord(x, y + 1);
    Particle *l = ParticleAtCoord(x - direction, y + 1);
    Particle *r = ParticleAtCoord(x + direction, y + 1);
    if (IsEmpty(b))
    {
        SwapParticles(x, y, x, y + 1);
    }
    else if (IsEmpty(l))
    {
        SwapParticles(x, y, x - direction, y + 1);
    }
    else if (IsEmpty(r))
    {
        SwapParticles(x, y, x + direction, y + 1);
    }
}
void ParticleWorld::SwapParticles(int x1, int y1, int x2, int y2) {
    Particle* tmp = ParticleAtCoord(x1, y1);
    _particles[CoordToIndex(x1, y1)] = ParticleAtCoord(x2, y2);
    _particles[CoordToIndex(x2, y2)] = tmp;
}