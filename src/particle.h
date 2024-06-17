#include <raylib.h>
#include <iostream>
#include <algorithm>
#include <vector>

double randomBetween(double a, double b);

typedef int Mat_Type;
#define t_air (Mat_Type)0
#define t_solid (Mat_Type)1
#define t_sand (Mat_Type)2
#define t_water (Mat_Type)3

#define color_air() WHITE
#define color_solid() ColorBrightness(BLACK, randomBetween(-.1f, .3f))
#define color_sand() ColorBrightness(YELLOW, randomBetween(-.5f, 0.0f))
#define color_water() ColorBrightness(BLUE, randomBetween(-0.2f, .2f))

#define GRAVITY 9.80f

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
    Vector2 const getVelocity() { return _velocity; };
    void setVelocity(Vector2 v) { _velocity = v; };
    Vector2 const getAcceleration() { return _acceleration; };
    void setAcceleration(Vector2 a) { _acceleration = a; };
    int const getX() { return _position.y; };
    int const getY() { return _position.x; };
    Color const getColor() { return _color; };
    void setColor(Color col) { _color = col; };
    Mat_Type const getType() { return _materialType; };

private:
    Vector2 _position = {0, 0};
    Vector2 _velocity = {0.0f, -2.0f};
    Vector2 _acceleration = {0.0f, GRAVITY};
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
    double const getDeltaTime() { return _deltaTime; };
    void setDeltaTime(double dt) { _deltaTime = dt; };
    void setCurrentTime(double t) { _currentTime = t; };

protected:
    bool InBounds(int x, int y) { return x >= 0 && y >= 0 && x < _width && y < _height; }
    bool IsEmpty(Particle *p) { return p != nullptr && p->getType() == t_air; }
    bool IsEmpty(int x, int y)
    {
        Particle *p = ParticleAtCoord(x, y);
        return IsEmpty(p);
    }
    bool IsEmptyOrWater(int x, int y)
    {
        Particle *p = ParticleAtCoord(x, y);
        return IsEmptyOrWater(p);
    }
    bool IsEmptyOrWater(Particle *p)
    {
        return (p != nullptr && (p->getType() == t_water || p->getType() == t_air));
    }
    void SwapParticles(int x1, int y1, int x2, int y2);
    void SwapParticles(int id1, int id2);
    void UpdateSand(int x, int y);
    void UpdateWater(int x, int y);
    void CommitChanges();
    void MoveParticle(int x1, int y1, int x2, int y2);

private:
    std::vector<std::pair<int, int>> _frameSwaps; // src, dest
    std::vector<Particle *> _particles;
    int _maxParticles;
    int _width;
    int _height;
    double _deltaTime;
    double _currentTime;
};

ParticleWorld::ParticleWorld(int width, int height)
{
    _maxParticles = width * height;
    _width = width;
    _height = height;
    for (int i = 0; i < _maxParticles; i++)
    {
        Particle *tmp = new Particle{t_air, color_air()};
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
                break;
            }
            case t_sand:
            {
                UpdateSand(x, y);
                break;
            }
            case t_solid:
            {
                break;
            }
            case t_water:
            {
                UpdateWater(x, y);
                break;
            }
            }
        }
    }
    CommitChanges();
}

void ParticleWorld::CommitChanges()
{
    for (int i = 0; i < _frameSwaps.size(); i++)
    {
        if (!IsEmptyOrWater(ParticleAtIndex(_frameSwaps[i].second)))
        {
            _frameSwaps[i] = _frameSwaps.back();
            _frameSwaps.pop_back();
            i--;
        }
    }

    std::sort(_frameSwaps.begin(), _frameSwaps.end(),
              [](auto &a, auto &b)
              { return a.second < b.second; });

    int iprev = 0;

    _frameSwaps.emplace_back(-1, -1);
    for (int i = 0; i < _frameSwaps.size() - 1; i++)
    {
        if (_frameSwaps[i + 1].second != _frameSwaps[i].second)
        {
            int rand = iprev + (((double)std::rand() / (RAND_MAX)) * (i - iprev));

            int dst = _frameSwaps[rand].first;
            int src = _frameSwaps[rand].second;

            SwapParticles(dst, src);

            iprev = i + 1;
        }
    }
    _frameSwaps.clear();
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
    if (!InBounds(x, y))
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
    if (!InBounds(x, y))
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
    double deltaPass = 0.2;
    Particle *p = ParticleAtCoord(x, y);
    Vector2 vel = p->getVelocity();
    Vector2 accel = p->getAcceleration();
    // calculate y velocity
    float xVelocity = vel.x + accel.x * _deltaTime;
    float yVelocity = vel.y + accel.y * _deltaTime;
    int xDelta = xVelocity;
    int yDelta = yVelocity;
    bool down = IsEmptyOrWater(x, y + 1) && InBounds(x, y + 1);
    bool downLeft = IsEmptyOrWater(x - 1, y + 1) && InBounds(x - 1, y + 1);
    bool downRight = IsEmptyOrWater(x + 1, y + 1) && InBounds(x + 1, y + 1);
    p->setVelocity(Vector2{(float)xVelocity, (float)yVelocity});

    if (downLeft && downRight)
    {
        downLeft = std::rand() % 2;
        downRight = !downLeft;
    }
    if (down)
    {
        int dstY = y;
        for (int i = 1; i <= yDelta; i++)
        {
            int t = y + i;
            if (IsEmptyOrWater(x, t) && InBounds(x, t))
            {
                dstY = t;
            }
            else
            {
                break;
            }
        }
        MoveParticle(x, y, x, dstY);
    }
    else if (downLeft)
    {
        MoveParticle(x, y, x - 1, y + yDelta);
    }
    else if (downRight)
    {
        MoveParticle(x, y, x + 1, y + yDelta);
    }
}

void ParticleWorld::UpdateWater(int x, int y)
{
    bool down = IsEmpty(x, y + 1) && InBounds(x, y + 1);
    bool left = IsEmpty(x - 1, y) && InBounds(x - 1, y);
    bool right = IsEmpty(x + 1, y) && InBounds(x + 1, y);
    bool downLeft = IsEmpty(x - 1, y + 1) && InBounds(x - 1, y + 1);
    bool downRight = IsEmpty(x + 1, y + 1) && InBounds(x + 1, y + 1);

    if (left && right)
    {
        left = std::rand() % 2;
        right = !left;
    }
    if (downLeft && downRight)
    {
        downLeft = std::rand() % 2;
        downRight = !downLeft;
    }
    if (down)
    {
        MoveParticle(x, y, x, y + 1);
    }
    else if (left)
    {
        MoveParticle(x, y, x - 1, y);
    }
    else if (right)
    {
        MoveParticle(x, y, x + 1, y);
    }
    else if (downLeft)
    {
        MoveParticle(x, y, x - 1, y + 1);
    }
    else if (downRight)
    {
        MoveParticle(x, y, x + 1, y + 1);
    }
}

void ParticleWorld::MoveParticle(int x1, int y1, int x2, int y2)
{
    _frameSwaps.emplace_back(CoordToIndex(x1, y1), CoordToIndex(x2, y2));
}

void ParticleWorld::SwapParticles(int x1, int y1, int x2, int y2)
{
    Particle *tmp = ParticleAtCoord(x1, y1);
    _particles[CoordToIndex(x1, y1)] = ParticleAtCoord(x2, y2);
    _particles[CoordToIndex(x2, y2)] = tmp;
}

void ParticleWorld::SwapParticles(int id1, int id2)
{
    Particle *tmp = ParticleAtIndex(id1);
    _particles[id1] = ParticleAtIndex(id2);
    _particles[id2] = tmp;
}

double randomBetween(double a, double b)
{
    double normalized = (double)std::rand() / RAND_MAX;
    normalized *= (b - a);
    return normalized + a;
}