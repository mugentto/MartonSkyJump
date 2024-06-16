
#include <sl.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib>   // untuk fungsi rand()
#include <ctime>     // untuk menginisialisasi seed rand()

// Fungsi alternatif untuk std::to_string
std::string to_string(int number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}

// Ukuran window
const int windowWidth = 800;
const int windowHeight = 400;
const float groundHeight = 50; // Tinggi tanah dari bawah layar
const float birdHeight = 99;  // Tinggi burung dari bawah layar

// Karakter ninja
struct Ninja {
    float x, y;
    float width, height;
    float velocityY;
    bool isJumping;
    bool isRolling; // Menandakan apakah ninja sedang bergulir
    std::vector<int> textures; // Menggunakan vector untuk menyimpan semua texture ID (lari)
    std::vector<int> ninjaRollTextures; // Texture ID untuk animasi bergulir
    int currentFrame; // Indeks frame saat ini
    float frameTime; // Waktu yang sudah berlalu sejak frame terakhir berubah
};

// Rintangan
struct Obstacle {
    float x, y;
    float width, height;
    float speed;
    bool isActive; // Menandakan apakah rintangan aktif (muncul di layar)
    int obstacleType; // 0 untuk dog, 1 untuk bird
    std::vector<int> dogTextures;  // Menggunakan vector untuk menyimpan semua texture ID untuk animasi dog
    std::vector<int> birdTextures; // Menggunakan vector untuk menyimpan semua texture ID untuk animasi bird
    int currentFrame; // Indeks frame saat ini
    float frameTime; // Waktu yang sudah berlalu sejak frame terakhir berubah
};

void resetObstacle(Obstacle& obstacle) {
    obstacle.x = windowWidth;
    obstacle.speed = 400;   // Kecepatan rintangan
    obstacle.currentFrame = 0; // Reset frame ke awal
    obstacle.frameTime = 0.0f; // Reset waktu frame
    obstacle.isActive = true; // Aktifkan rintangan
    // Memilih secara acak antara dog atau bird
    obstacle.obstacleType = std::rand() % 2; // 0 untuk dog, 1 untuk bird
    // Menentukan posisi y berdasarkan tipe rintangan
    if (obstacle.obstacleType == 0) {
        obstacle.y = groundHeight; // Posisi anjing di dekat tanah
    }
    else {
        obstacle.y = birdHeight;   // Posisi burung di udara
    }
}

// Fungsi untuk mengecek tabrakan berdasarkan tipe rintangan
bool checkCollision(Ninja ninja, Obstacle obstacle) {
    if (obstacle.isActive) {
        if (obstacle.obstacleType == 0) { // Dog
            return ninja.x < obstacle.x + obstacle.width &&
                ninja.x + ninja.width > obstacle.x &&
                ninja.y < obstacle.y + obstacle.height &&
                ninja.y + ninja.height > obstacle.y;
        }
        else { // Bird
            // Ninja bergulir tidak bisa menabrak burung
            if (!ninja.isRolling) {
                return ninja.x < obstacle.x + obstacle.width &&
                    ninja.x + ninja.width > obstacle.x &&
                    ninja.y < obstacle.y + obstacle.height &&
                    ninja.y + ninja.height > obstacle.y;
            }
        }
    }
    return false;
}

int main() {
    // Inisialisasi window
    slWindow(windowWidth, windowHeight, "Ninja Jump", false);

    // Muat gambar ninja (lari)
    std::vector<int> ninjaTextures;
    for (int i = 1; i <= 10; ++i) {
        std::string texturePath = "assets/ninjarun/ninja_run" + to_string(i) + ".png";
        ninjaTextures.push_back(slLoadTexture(texturePath.c_str()));
    }

    // Muat gambar ninja (bergulir)
    std::vector<int> ninjaRollTextures;
    for (int i = 1; i <= 7; ++i) {
        std::string texturePath = "assets/ninjaroll/ninjaroll" + to_string(i) + ".png";
        ninjaRollTextures.push_back(slLoadTexture(texturePath.c_str()));
    }

    // Muat gambar dog
    std::vector<int> dogTextures;
    for (int i = 3; i <= 8; ++i) {
        std::string texturePath = "assets/dog/dog" + to_string(i) + ".png";
        dogTextures.push_back(slLoadTexture(texturePath.c_str()));
    }

    // Muat gambar bird
    std::vector<int> birdTextures;
    for (int i = 1; i <= 8; ++i) {
        std::string texturePath = "assets/bird/bird" + to_string(i) + ".png";
        birdTextures.push_back(slLoadTexture(texturePath.c_str()));
    }

    // Inisialisasi ninja dengan ukuran yang disesuaikan
    float ninjaWidth = 80;  // Sesuaikan lebar ninja
    float ninjaHeight = 50; // Sesuaikan tinggi ninja
    Ninja ninja = { 100, groundHeight, ninjaWidth, ninjaHeight, 0, false, false, ninjaTextures, ninjaRollTextures, 0, 0.0f };

    // Inisialisasi rintangan
    Obstacle obstacle = { windowWidth, groundHeight, 60, 40, 400, false, 0, dogTextures, birdTextures, 0, 0.0f };

    // Gravitasi dan kecepatan lompat
    const float gravity = -800; // Gravitasi negatif untuk lompat ke bawah
    const float jumpVelocity = 400; // Kecepatan lompat ke bawah

    bool gameRunning = true;

    // Waktu antara frame animasi
    const float animationFrameTime = 0.1f; // Ubah frame setiap 0.1 detik

    // Seed untuk fungsi rand() (biasanya dijalankan sekali di awal program)
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Loop utama
    while (!slShouldClose() && !slGetKey(SL_KEY_ESCAPE) && gameRunning) {
        // Mengatur kecepatan per frame
        float dt = slGetDeltaTime();

        // Mengatur animasi bergulir ninja jika tombol 'R' ditekan
        if (slGetKey('R') && !ninja.isRolling) {
            ninja.isRolling = true;
            ninja.currentFrame = 0; // Reset frame bergulir ke awal
            ninja.frameTime = 0.0f; // Reset waktu frame
        }

        // Menghentikan animasi bergulir jika tombol 'R' dilepas
        if (!slGetKey('R') && ninja.isRolling) {
            ninja.isRolling = false;
        }

        // Menggerakkan rintangan jika tidak aktif (di luar layar)
        if (!obstacle.isActive) {
            resetObstacle(obstacle);
        }

        // Menggerakkan rintangan
        obstacle.x -= obstacle.speed * dt;

        // Menonaktifkan rintangan jika keluar dari layar
        if (obstacle.x < -obstacle.width) {
            obstacle.isActive = false;
        }

        // Melakukan lompatan ninja
        if (slGetKey(' ') && !ninja.isJumping) {
            ninja.isJumping = true;
            ninja.velocityY = jumpVelocity;
        }

        // Update posisi ninja saat melompat
        if (ninja.isJumping) {
            ninja.velocityY += gravity * dt;
            ninja.y += ninja.velocityY * dt;

            // Mendarat kembali di "tanah" (sekarang di atas)
            if (ninja.y <= groundHeight) {
                ninja.y = groundHeight;
                ninja.isJumping = false;
                ninja.velocityY = 0;
            }
        }

        // Update frame animasi ninja (lari atau bergulir)
        if (!ninja.isRolling) {
            ninja.frameTime += dt;
            if (ninja.frameTime >= animationFrameTime) {
                ninja.currentFrame = (ninja.currentFrame + 1) % ninja.textures.size();
                ninja.frameTime = 0.0f;
            }
        }
        else {
            ninja.frameTime += dt;
            if (ninja.frameTime >= animationFrameTime) {
                ninja.currentFrame = (ninja.currentFrame + 1) % ninja.ninjaRollTextures.size();
                ninja.frameTime = 0.0f;
            }
        }

        // Update frame animasi obstacle (dog atau bird)
        if (obstacle.isActive) {
            obstacle.frameTime += dt;
            if (obstacle.frameTime >= animationFrameTime) {
                obstacle.currentFrame = (obstacle.currentFrame + 1) % (obstacle.obstacleType == 0 ? obstacle.dogTextures.size() : obstacle.birdTextures.size());
                obstacle.frameTime = 0.0f;
            }
        }

        // Menghentikan permainan jika terjadi tabrakan antara ninja dan rintangan
        if (checkCollision(ninja, obstacle)) {
            gameRunning = false;
        }

        // Membersihkan layar dengan menggambar latar belakang
        slSetForeColor(0.9f, 0.9f, 0.9f, 1.0f);
        slRectangleFill(windowWidth / 2, windowHeight / 2, windowWidth, windowHeight);

        // Menggambar ninja dengan gambar sprite sesuai dengan kondisi
        slSetForeColor(1.0f, 1.0f, 1.0f, 1.0f); // Atur warna putih untuk gambar
        if (!ninja.isRolling) {
            slSprite(ninja.textures[ninja.currentFrame], ninja.x + ninja.width / 2, ninja.y + ninja.height / 2, ninja.width, ninja.height);
        }
        else {
            slSprite(ninja.ninjaRollTextures[ninja.currentFrame], ninja.x + ninja.width / 2, ninja.y + ninja.height / 2, ninja.width, ninja.height);
        }

        // Menggambar rintangan (dog atau bird) dengan gambar sprite jika aktif
        if (obstacle.isActive) {
            if (obstacle.obstacleType == 0) {
                slSprite(obstacle.dogTextures[obstacle.currentFrame], obstacle.x + obstacle.width / 2, obstacle.y + obstacle.height / 2, obstacle.width, obstacle.height);
            }
            else {
                slSprite(obstacle.birdTextures[obstacle.currentFrame], obstacle.x + obstacle.width / 2, obstacle.y + obstacle.height / 2, obstacle.width, obstacle.height);
            }
        }

        // Menampilkan hasil gambar di layar
        slRender();
    }
    // Menutup window
    slClose();
    return 0;
}