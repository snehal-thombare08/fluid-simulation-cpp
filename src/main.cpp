#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <algorithm>

const int WIDTH = 800, HEIGHT = 600;
const float H = 20.f;          // smoothing radius
const float H2 = H * H;
const float REST_DENSITY = 10.f;
const float GAS_CONSTANT = 100.f;
const float VISCOSITY = 0.1f;
const float GRAVITY = 200.f;
const float DAMPING = 0.3f;
const float PARTICLE_RADIUS = 5.f;
const float MASS = 1.f;
const float DT = 0.005f;

struct Particle {
    float x, y;
    float vx, vy;
    float fx, fy;
    float density, pressure;
};

// SPH Kernel: Poly6
float poly6(float r2) {
    if (r2 >= H2) return 0.f;
    float t = H2 - r2;
    return (315.f / (64.f * 3.14159f * powf(H, 9.f))) * t * t * t;
}

// SPH Kernel Gradient: Spiky
float spiky_grad(float r) {
    if (r >= H || r < 1e-6f) return 0.f;
    float t = H - r;
    return -(45.f / (3.14159f * powf(H, 6.f))) * t * t;
}

// Viscosity Laplacian
float visc_laplacian(float r) {
    if (r >= H) return 0.f;
    return (45.f / (3.14159f * powf(H, 6.f))) * (H - r);
}

// Color based on speed
sf::Color speedColor(float vx, float vy) {
    float speed = sqrtf(vx * vx + vy * vy);
    float t = std::min(speed / 300.f, 1.f);
    // blue -> cyan -> green -> yellow -> red
    if (t < 0.25f) {
        float s = t / 0.25f;
        return sf::Color(0, (uint8_t)(s * 255), 255);
    } else if (t < 0.5f) {
        float s = (t - 0.25f) / 0.25f;
        return sf::Color(0, 255, (uint8_t)((1 - s) * 255));
    } else if (t < 0.75f) {
        float s = (t - 0.5f) / 0.25f;
        return sf::Color((uint8_t)(s * 255), 255, 0);
    } else {
        float s = (t - 0.75f) / 0.25f;
        return sf::Color(255, (uint8_t)((1 - s) * 255), 0);
    }
}

std::vector<Particle> particles;

void spawnParticles(float cx, float cy, int count = 30) {
    for (int i = 0; i < count; i++) {
        Particle p;
        int row = i / 6;
        int col = i % 6;
        p.x = cx + col * 9.f - 22.f;
        p.y = cy + row * 9.f - 22.f;
        p.vx = ((rand() % 100) - 50) * 0.5f;
        p.vy = ((rand() % 100) - 50) * 0.5f;
        p.fx = p.fy = 0;
        p.density = p.pressure = 0;
        particles.push_back(p);
    }
}

void computeDensityPressure() {
    for (auto& pi : particles) {
        pi.density = 0.f;
        for (auto& pj : particles) {
            float dx = pj.x - pi.x;
            float dy = pj.y - pi.y;
            float r2 = dx * dx + dy * dy;
            pi.density += MASS * poly6(r2);
        }
        pi.pressure = GAS_CONSTANT * (pi.density - REST_DENSITY);
    }
}

void computeForces() {
    for (auto& pi : particles) {
        pi.fx = 0.f;
        pi.fy = GRAVITY * pi.density;
        for (auto& pj : particles) {
            if (&pi == &pj) continue;
            float dx = pj.x - pi.x;
            float dy = pj.y - pi.y;
            float r2 = dx * dx + dy * dy;
            float r = sqrtf(r2);
            if (r < H && r > 1e-6f) {
                // Pressure force
                float pterm = -MASS * (pi.pressure + pj.pressure) / (2.f * pj.density) * spiky_grad(r);
                pi.fx += pterm * dx / r;
                pi.fy += pterm * dy / r;
                // Viscosity force
                float vterm = VISCOSITY * MASS * visc_laplacian(r) / pj.density;
                pi.fx += vterm * (pj.vx - pi.vx);
                pi.fy += vterm * (pj.vy - pi.vy);
            }
        }
    }
}

void integrate() {
    for (auto& p : particles) {
        if (p.density > 1e-6f) {
            p.vx += DT * p.fx / p.density;
            p.vy += DT * p.fy / p.density;
        }
        p.x += DT * p.vx;
        p.y += DT * p.vy;

        // Boundary
        if (p.x < PARTICLE_RADIUS) {
            p.x = PARTICLE_RADIUS;
            p.vx *= -DAMPING;
        }
        if (p.x > WIDTH - PARTICLE_RADIUS) {
            p.x = WIDTH - PARTICLE_RADIUS;
            p.vx *= -DAMPING;
        }
        if (p.y < PARTICLE_RADIUS) {
            p.y = PARTICLE_RADIUS;
            p.vy *= -DAMPING;
        }
        if (p.y > HEIGHT - PARTICLE_RADIUS) {
            p.y = HEIGHT - PARTICLE_RADIUS;
            p.vy *= -DAMPING;
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned)WIDTH, (unsigned)HEIGHT}), "Fluid Simulation - SPH");
    window.setFramerateLimit(60);

    sf::CircleShape circle(PARTICLE_RADIUS);
    circle.setOrigin({PARTICLE_RADIUS, PARTICLE_RADIUS});

    // Spawn initial particles
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            spawnParticles(200.f + i * 60.f, 100.f + j * 40.f, 6);

    // FPS text
    sf::Font font;
    bool hasFont = font.openFromFile("C:/Windows/Fonts/arial.ttf");
    sf::Text fpsText(font);
    fpsText.setCharacterSize(16);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition({10.f, 10.f});

    sf::Text infoText(font);
    infoText.setCharacterSize(14);
    infoText.setFillColor(sf::Color(200, 200, 200));
    infoText.setPosition({10.f, 570.f});
    infoText.setString("Left Click: Add particles | Right Click: Clear");

    sf::Clock clock;
    sf::Clock fpsClock;
    int frameCount = 0;
    float fps = 0.f;

    bool mouseHeld = false;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mb->button == sf::Mouse::Button::Right) {
                    particles.clear();
                }
            }
        }

        // Hold left mouse to spawn
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            auto mp = sf::Mouse::getPosition(window);
            if (mp.x > 0 && mp.x < WIDTH && mp.y > 0 && mp.y < HEIGHT) {
                if (particles.size() < 800)
                    spawnParticles((float)mp.x, (float)mp.y, 5);
            }
        }

        // Physics steps per frame
        for (int step = 0; step < 3; step++) {
            computeDensityPressure();
            computeForces();
            integrate();
        }

        // FPS
        frameCount++;
        float elapsed = fpsClock.getElapsedTime().asSeconds();
        if (elapsed >= 0.5f) {
            fps = frameCount / elapsed;
            frameCount = 0;
            fpsClock.restart();
        }
        if (hasFont) {
            fpsText.setString("FPS: " + std::to_string((int)fps) + "  Particles: " + std::to_string(particles.size()));
        }

        window.clear(sf::Color(8, 8, 20));

        for (auto& p : particles) {
            sf::Color c = speedColor(p.vx, p.vy);
            c.a = 220;
            circle.setFillColor(c);
            circle.setPosition({p.x, p.y});
            window.draw(circle);
        }

        if (hasFont) {
            window.draw(fpsText);
            window.draw(infoText);
        }

        window.display();
    }
    return 0;
}
