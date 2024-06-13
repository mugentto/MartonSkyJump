#include <sl.h>
#include <iostream>
#include <vector>
#include <sstream>

// Fungsi alternatif untuk std::to_string
std::string to_string(int number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}

// Ukuran window
const int windowWidth = 800;
const int windowHeight = 400;
const float topMargin = 100; // Margin dari atas layar

// Karakter dinosaurus
struct Dino {
    float x, y;
    float width, height;
    float velocityY;
    bool isJumping;
    std::vector<int> textures; // Menggunakan vector untuk menyimpan semua texture ID
    int currentFrame; // Indeks frame saat ini
    float frameTime; // Waktu yang sudah berlalu sejak frame terakhir berubah
};

// Rintangan
struct Obstacle {
    float x, y;
    float width, height;
    float speed;
    int texture; // Tambahkan variabel untuk menyimpan texture ID
};

void resetObstacle(Obstacle& obstacle) {
    obstacle.x = windowWidth;
    obstacle.y = topMargin; // Pindahkan rintangan ke atas layar sejajar dengan dino
    obstacle.speed = 300;
}

// Fungsi untuk mengecek tabrakan
bool checkCollision(Dino dino, Obstacle obstacle) {
    return dino.x < obstacle.x + obstacle.width &&
        dino.x + dino.width > obstacle.x &&
        dino.y < obstacle.y + obstacle.height &&
        dino.y + dino.height > obstacle.y;
}

int main() {
    // Inisialisasi window
    slWindow(windowWidth, windowHeight, "Dino Jump", false);

    // Muat gambar
    std::vector<int> dinoTextures;
    for (int i = 1; i <= 10; ++i) {
        std::string texturePath = "assets/ninja_run " + to_string(i) + ".png";
        dinoTextures.push_back(slLoadTexture(texturePath.c_str()));
    }
    int obstacleTexture = slLoadTexture("assets/stone.png");

    // Inisialisasi dino dengan ukuran yang disesuaikan
    float dinoWidth = 100;  // Sesuaikan lebar ninja
    float dinoHeight = 60; // Sesuaikan tinggi ninja
    Dino dino = { 100, topMargin, dinoWidth, dinoHeight, 0, false, dinoTextures, 0, 0.0f };

    // Inisialisasi rintangan
    Obstacle obstacle = { windowWidth, topMargin, 20, 50, 300, obstacleTexture }; // Pindahkan rintangan ke atas layar sejajar dengan dino

    // Gravitasi dan kecepatan lompat
    const float gravity = -800; // Gravitasi negatif untuk lompat ke bawah
    const float jumpVelocity = 400; // Kecepatan lompat ke bawah

    bool gameRunning = true;

    // Waktu antara frame animasi
    const float animationFrameTime = 0.1f; // Ubah frame setiap 0.1 detik

    // Loop utama
    while (!slShouldClose() && !slGetKey(SL_KEY_ESCAPE) && gameRunning) {
        // Mengatur kecepatan per frame
        float dt = slGetDeltaTime();

        // Menggerakkan rintangan
        obstacle.x -= obstacle.speed * dt;

        // Mereset posisi rintangan jika keluar dari layar
        if (obstacle.x < -obstacle.width) {
            resetObstacle(obstacle);
        }

        // Melakukan lompatan dino
        if (slGetKey(' ') && !dino.isJumping) {
            dino.isJumping = true;
            dino.velocityY = jumpVelocity;
        }

        // Update posisi dino saat melompat
        if (dino.isJumping) {
            dino.velocityY += gravity * dt;
            dino.y += dino.velocityY * dt;

            // Mendarat kembali di "tanah" (sekarang di atas)
            if (dino.y <= topMargin) {
                dino.y = topMargin;
                dino.isJumping = false;
                dino.velocityY = 0;
            }
        }

        // Cek tabrakan
        if (checkCollision(dino, obstacle)) {
            gameRunning = false;
        }

        // Update frame animasi
        dino.frameTime += dt;
        if (dino.frameTime >= animationFrameTime) {
            dino.currentFrame = (dino.currentFrame + 1) % dino.textures.size();
            dino.frameTime = 0.0f;
        }

        // Membersihkan layar dengan menggambar latar belakang
        slSetForeColor(0.9f, 0.9f, 0.9f, 1.0f);
        slRectangleFill(windowWidth / 2, windowHeight / 2, windowWidth, windowHeight);

        // Menggambar dino dengan gambar sprite
        slSetForeColor(1.0f, 1.0f, 1.0f, 1.0f); // Atur warna putih untuk gambar
        slSprite(dino.textures[dino.currentFrame], dino.x + dino.width / 2, dino.y + dino.height / 2, dino.width, dino.height);

        // Menggambar rintangan dengan gambar sprite
        slSprite(obstacle.texture, obstacle.x + obstacle.width / 2, obstacle.y + obstacle.height / 2, obstacle.width, obstacle.height);

        // Menampilkan hasil gambar di layar
        slRender();
    }

    // Menutup window
    slClose();
    return 0;
}
