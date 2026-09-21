
#include <Arduino.h>
#include <AyresWiFiManager.h>
#include "env.h"

// Instance du gestionnaire WiFi
AyresWiFiManager wifi;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("=========================================");
  Serial.println("MicroCoaster - WiFiManager v2.0.0");
  Serial.println("=========================================");
  Serial.println();

  // Configuration du point d'accès
  Serial.println("Configuration du point d'accès...");
  wifi.setAPCredentials(ESP_WIFI_SSID, ESP_WIFI_PASSWORD);
  Serial.print("   ├─ SSID: ");
  Serial.println(ESP_WIFI_SSID);
  Serial.print("   └─ Mot de passe: ");
  Serial.println(ESP_WIFI_PASSWORD);
  
  // Configuration des timeouts
  Serial.println("Configuration des timeouts...");
  wifi.setPortalTimeout(3600);  // 60 minutes (très long pour debug)
  wifi.setAPClientCheck(true);  // Ne pas fermer si des clients sont connectés
  wifi.setWebClientCheck(true); // Chaque requête HTTP remet à zéro le timer
  Serial.println("   ├─ Timeout portail: 60 minutes");
  Serial.println("   ├─ Vérification clients: activée");
  Serial.println("   └─ Vérification requêtes web: activée");
  
  // Configuration optionnelle
  Serial.println("Configuration avancée...");
  wifi.setCaptivePortal(true); // Activer les redirections pour portail captif
  Serial.println("   ├─ Portail captif: activé");
  
  // Configuration hybride : première connexion + production
  wifi.setFallbackPolicy(AyresWiFiManager::FallbackPolicy::ON_FAIL);
  wifi.setAutoReconnect(true); // Reconnexion automatique en cas de déconnexion
  Serial.println("   ├─ Politique de secours: ON_FAIL");
  Serial.println("   └─ Reconnexion automatique: activée");
  
  // Protection des fichiers critiques (empêche leur suppression accidentelle)
  wifi.setProtectedJsons({"/wifi.json"}); // Protège le fichier de configuration WiFi
  Serial.println("Protection fichiers: /wifi.json");
  
  // Activation du bouton de secours (GPIO 0 par défaut)
  wifi.enableButtonPortal(true); // Bouton 2-5s = ouvre portail, ≥5s = efface identifiants
  Serial.println("Bouton de secours: GPIO 0 (2-5s = portail, ≥5s = reset)");
  
  // Initialisation du WiFi Manager
  Serial.println();
  Serial.println("Initialisation du WiFi Manager...");
  wifi.begin();  // Monte le système de fichiers, charge /wifi.json si présent
  Serial.println("Système de fichiers LittleFS monté");
  Serial.println("Recherche du fichier de configuration /wifi.json...");
  
  Serial.println("Tentative de connexion WiFi...");
  wifi.run();    // Essaie de se connecter en STA; si ça échoue, applique la politique de fallback
  
  // Vérification du statut après initialisation
  delay(2000); // Attendre un peu pour que la connexion se stabilise
  
  if (wifi.isConnected()) {
    Serial.println("[OK] Connexion WiFi réussie !");
    Serial.println("IP: " + WiFi.localIP().toString());
    Serial.println("Mode: Client WiFi (STA)");
  } else {
    Serial.println("[!] Connexion WiFi échouée");
    Serial.println("Ouverture du portail de configuration...");
    Serial.println("Point d'accès: WifiManager-MicroCoaster");
    Serial.println("IP du portail: 192.168.4.1");
    Serial.println("Connectez-vous au WiFi puis allez sur http://192.168.4.1");
  }
  
  Serial.println();
  Serial.println("[OK] Initialisation terminée !");
  Serial.println("=========================================");
}

void loop() {
  wifi.update(); // Sert HTTP/DNS et gère les timeouts/LED
  
  // Affichage du statut de connexion (toutes les 30 secondes)
  static unsigned long lastStatusCheck = 0;
  static bool lastConnectionState = false;
  
  if (millis() - lastStatusCheck > 30000) {
    lastStatusCheck = millis();
    bool currentState = wifi.isConnected();
    
    if (currentState) {
      Serial.println("[UP] WiFi connecté - IP: " + WiFi.localIP().toString() + 
                     " | Signal: " + String(WiFi.RSSI()) + " dBm");
    } else {
      Serial.println("[DOWN] WiFi déconnecté - Portail de configuration actif sur 192.168.4.1");
    }
    
    // Détecter les changements d'état
    if (currentState != lastConnectionState) {
      if (currentState) {
        Serial.println("Connexion WiFi établie !");
      } else {
        Serial.println("[!] Connexion WiFi perdue, basculement en mode portail...");
      }
      lastConnectionState = currentState;
    }
  }
  
  // Votre code principal ici
  if (wifi.isConnected()) {
    // Code à exécuter quand connecté au WiFi
    // Par exemple: envoi de données, synchronisation, etc.
  }
  
  delay(100);
}