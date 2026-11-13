// Agence de voyage - Cotonou : gestion des passagers, vols, reservations et paiements
// Compilation : g++ -std=c++17 -Wall -Wextra -o agence agence_voyage.cpp

#include <iostream>
#include <string>
#include <map>
#include <limits>
#include <stdexcept>
#include <iomanip>

using namespace std;

// ======================= Enumerations =======================
enum class StatutReservation { EnAttente, Validee, Annulee };
enum class StatutRemboursement { Aucun, EnAttente, Rembourse };

string toString(StatutReservation s) {
    switch (s) {
        case StatutReservation::EnAttente: return "En attente";
        case StatutReservation::Validee:   return "Validee";
        case StatutReservation::Annulee:   return "Annulee";
    }
    return "?";
}

string toString(StatutRemboursement s) {
    switch (s) {
        case StatutRemboursement::Aucun:     return "Aucun";
        case StatutRemboursement::EnAttente: return "En attente";
        case StatutRemboursement::Rembourse: return "Rembourse";
    }
    return "?";
}

// ======================= Passager =======================
class Passager {
private:
    int id;
    string nom;
    string prenom;
    string telephone;
    string email;
    string numeroPasseport;

public:
    Passager() : id(0) {}
    Passager(int id, const string& nom, const string& prenom, const string& telephone,
             const string& email, const string& passeport)
        : id(id), nom(nom), prenom(prenom), telephone(telephone),
          email(email), numeroPasseport(passeport) {}

    int getId() const { return id; }
    string getNom() const { return nom; }
    string getPrenom() const { return prenom; }
    string getNomComplet() const { return prenom + " " + nom; }
    string getTelephone() const { return telephone; }
    string getEmail() const { return email; }
    string getPasseport() const { return numeroPasseport; }

    void modifier(const string& n, const string& p, const string& tel, const string& mail) {
        nom = n; prenom = p; telephone = tel; email = mail;
    }

    void afficher() const {
        cout << "[Passager " << id << "] " << getNomComplet()
             << " | Tel: " << telephone << " | Email: " << email
             << " | Passeport: " << numeroPasseport << "\n";
    }
};

// ======================= Vol =======================
class Vol {
private:
    int id;
    string numeroVol;
    string depart;
    string destination;
    string dateDepart;   // ex: 2026-12-20
    string heureDepart;  // ex: 08:30
    int capacite;
    double prix;

public:
    Vol() : id(0), capacite(0), prix(0.0) {}
    Vol(int id, const string& numero, const string& depart, const string& destination,
        const string& date, const string& heure, int capacite, double prix)
        : id(id), numeroVol(numero), depart(depart), destination(destination),
          dateDepart(date), heureDepart(heure), capacite(capacite), prix(prix) {}

    int getId() const { return id; }
    string getNumeroVol() const { return numeroVol; }
    int getCapacite() const { return capacite; }
    double getPrix() const { return prix; }

    void afficher(int placesOccupees) const {
        cout << "[Vol " << id << "] " << numeroVol << " | " << depart << " -> " << destination
             << " | " << dateDepart << " " << heureDepart
             << " | Prix: " << fixed << setprecision(0) << prix << " FCFA"
             << " | Places: " << placesOccupees << "/" << capacite << "\n";
    }
};

// ======================= Paiement =======================
class Paiement {
private:
    int id;
    int idReservation;
    double montant;
    string modePaiement;
    StatutRemboursement statutRemboursement;

public:
    Paiement() : id(0), idReservation(0), montant(0.0),
                 statutRemboursement(StatutRemboursement::Aucun) {}
    Paiement(int id, int idReservation, double montant, const string& mode)
        : id(id), idReservation(idReservation), montant(montant), modePaiement(mode),
          statutRemboursement(StatutRemboursement::Aucun) {}

    int getId() const { return id; }
    int getIdReservation() const { return idReservation; }
    double getMontant() const { return montant; }
    StatutRemboursement getStatutRemboursement() const { return statutRemboursement; }

    void demanderRemboursement() {
        if (statutRemboursement == StatutRemboursement::Aucun)
            statutRemboursement = StatutRemboursement::EnAttente;
    }

    void rembourser() {
        if (statutRemboursement != StatutRemboursement::EnAttente)
            throw runtime_error("Aucun remboursement en attente pour ce paiement.");
        statutRemboursement = StatutRemboursement::Rembourse;
    }

    void afficher() const {
        cout << "[Paiement " << id << "] Reservation " << idReservation
             << " | " << fixed << setprecision(0) << montant << " FCFA"
             << " | Mode: " << modePaiement
             << " | Remboursement: " << toString(statutRemboursement) << "\n";
    }
};

// ======================= Reservation =======================
class Reservation {
private:
    int id;
    int idPassager;
    int idVol;
    StatutReservation statut;
    bool payee;

public:
    Reservation() : id(0), idPassager(0), idVol(0),
                    statut(StatutReservation::EnAttente), payee(false) {}
    Reservation(int id, int idPassager, int idVol)
        : id(id), idPassager(idPassager), idVol(idVol),
          statut(StatutReservation::EnAttente), payee(false) {}

    int getId() const { return id; }
    int getIdPassager() const { return idPassager; }
    int getIdVol() const { return idVol; }
    StatutReservation getStatut() const { return statut; }
    bool estPayee() const { return payee; }
    bool estActive() const { return statut != StatutReservation::Annulee; }

    void valider() {
        if (statut == StatutReservation::Annulee)
            throw runtime_error("Une reservation annulee ne peut plus etre utilisee.");
        if (statut == StatutReservation::Validee)
            throw runtime_error("Reservation deja validee.");
        statut = StatutReservation::Validee;
    }

    void annuler() {
        if (statut == StatutReservation::Annulee)
            throw runtime_error("Reservation deja annulee.");
        statut = StatutReservation::Annulee;
    }

    void marquerPayee() {
        if (statut == StatutReservation::Annulee)
            throw runtime_error("Une reservation annulee ne peut plus etre utilisee.");
        if (payee)
            throw runtime_error("Reservation deja payee.");
        payee = true;
    }

    void afficher() const {
        cout << "[Reservation " << id << "] Passager " << idPassager << " | Vol " << idVol
             << " | Statut: " << toString(statut)
             << " | Payee: " << (payee ? "oui" : "non") << "\n";
    }
};

// ======================= Agence (gestionnaire) =======================
class Agence {
private:
    string nom;
    map<int, Passager> passagers;
    map<int, Vol> vols;
    map<int, Reservation> reservations;
    map<int, Paiement> paiements;
    int prochainIdPassager, prochainIdVol, prochainIdReservation, prochainIdPaiement;

    Passager& trouverPassager(int id) {
        auto it = passagers.find(id);
        if (it == passagers.end()) throw runtime_error("Passager introuvable (doit etre enregistre).");
        return it->second;
    }
    Vol& trouverVol(int id) {
        auto it = vols.find(id);
        if (it == vols.end()) throw runtime_error("Vol introuvable.");
        return it->second;
    }
    Reservation& trouverReservation(int id) {
        auto it = reservations.find(id);
        if (it == reservations.end()) throw runtime_error("Reservation introuvable.");
        return it->second;
    }
    Paiement* trouverPaiementParReservation(int idReservation) {
        for (auto& p : paiements)
            if (p.second.getIdReservation() == idReservation) return &p.second;
        return nullptr;
    }

public:
    explicit Agence(const string& nom)
        : nom(nom), prochainIdPassager(1), prochainIdVol(1),
          prochainIdReservation(1), prochainIdPaiement(1) {}

    string getNom() const { return nom; }

    int placesOccupees(int idVol) const {
        int n = 0;
        for (const auto& r : reservations)
            if (r.second.getIdVol() == idVol && r.second.estActive()) ++n;
        return n;
    }

    // ---------- Passagers ----------
    int ajouterPassager(const string& nomP, const string& prenom, const string& tel,
                        const string& email, const string& passeport) {
        if (nomP.empty() || prenom.empty() || passeport.empty())
            throw runtime_error("Nom, prenom et passeport sont obligatoires.");
        for (const auto& p : passagers)
            if (p.second.getPasseport() == passeport)
                throw runtime_error("Un passager avec ce numero de passeport existe deja.");
        int id = prochainIdPassager++;
        passagers[id] = Passager(id, nomP, prenom, tel, email, passeport);
        return id;
    }

    void modifierPassager(int id, const string& n, const string& p,
                          const string& tel, const string& mail) {
        trouverPassager(id).modifier(n, p, tel, mail);
    }

    void listerPassagers() const {
        if (passagers.empty()) { cout << "Aucun passager.\n"; return; }
        for (const auto& p : passagers) p.second.afficher();
    }

    // ---------- Vols ----------
    int ajouterVol(const string& numero, const string& depart, const string& dest,
                   const string& date, const string& heure, int capacite, double prix) {
        if (numero.empty()) throw runtime_error("Le numero de vol est obligatoire.");
        if (capacite <= 0) throw runtime_error("La capacite doit etre positive.");
        if (prix < 0) throw runtime_error("Le prix ne peut pas etre negatif.");
        for (const auto& v : vols)
            if (v.second.getNumeroVol() == numero)
                throw runtime_error("Ce numero de vol existe deja.");
        int id = prochainIdVol++;
        vols[id] = Vol(id, numero, depart, dest, date, heure, capacite, prix);
        return id;
    }

    void listerVols() const {
        if (vols.empty()) { cout << "Aucun vol.\n"; return; }
        for (const auto& v : vols) v.second.afficher(placesOccupees(v.first));
    }

    // ---------- Reservations ----------
    int reserver(int idPassager, int idVol) {
        trouverPassager(idPassager);                       // regle 1
        const Vol& vol = trouverVol(idVol);
        for (const auto& r : reservations)                 // regle 3
            if (r.second.getIdPassager() == idPassager &&
                r.second.getIdVol() == idVol && r.second.estActive())
                throw runtime_error("Ce passager a deja une reservation pour ce vol.");
        if (placesOccupees(idVol) >= vol.getCapacite())    // regle 2
            throw runtime_error("Vol complet : capacite atteinte.");
        int id = prochainIdReservation++;
        reservations[id] = Reservation(id, idPassager, idVol);  // regle 4
        return id;
    }

    void validerReservation(int id) {                      // regle 6
        trouverReservation(id).valider();
    }

    void payerReservation(int idReservation, const string& mode) {
        Reservation& r = trouverReservation(idReservation);
        double montant = trouverVol(r.getIdVol()).getPrix();
        r.marquerPayee();
        int id = prochainIdPaiement++;
        paiements[id] = Paiement(id, idReservation, montant, mode);
    }

    void annulerReservation(int id) {                      // regles 5 et 7
        Reservation& r = trouverReservation(id);
        r.annuler();
        if (r.estPayee()) {
            Paiement* p = trouverPaiementParReservation(id);
            if (p) p->demanderRemboursement();
        }
    }

    void rembourserReservation(int idReservation) {
        Reservation& r = trouverReservation(idReservation);
        if (r.getStatut() != StatutReservation::Annulee)
            throw runtime_error("Seule une reservation annulee peut etre remboursee.");
        Paiement* p = trouverPaiementParReservation(idReservation);
        if (!p) throw runtime_error("Aucun paiement associe a cette reservation.");
        p->rembourser();
    }

    void listerReservations() const {
        if (reservations.empty()) { cout << "Aucune reservation.\n"; return; }
        for (const auto& r : reservations) r.second.afficher();
    }

    void listerPaiements() const {
        if (paiements.empty()) { cout << "Aucun paiement.\n"; return; }
        for (const auto& p : paiements) p.second.afficher();
    }
};

// ======================= Saisie securisee =======================
bool lireLigne(const string& invite, string& out) {
    cout << invite;
    return static_cast<bool>(getline(cin, out));
}

string lireTexte(const string& invite) {
    string s;
    if (!lireLigne(invite, s)) throw runtime_error("Fin de saisie.");
    return s;
}

int lireEntier(const string& invite) {
    while (true) {
        string s = lireTexte(invite);
        try {
            size_t pos = 0;
            int v = stoi(s, &pos);
            if (pos == s.size()) return v;
        } catch (const exception&) {}
        cout << "Entree invalide, veuillez saisir un nombre entier.\n";
    }
}

double lireReel(const string& invite) {
    while (true) {
        string s = lireTexte(invite);
        try {
            size_t pos = 0;
            double v = stod(s, &pos);
            if (pos == s.size()) return v;
        } catch (const exception&) {}
        cout << "Entree invalide, veuillez saisir un nombre.\n";
    }
}

void afficherMenu() {
    cout << "\n========== AGENCE DE VOYAGE - COTONOU ==========\n"
         << " 1. Ajouter un passager\n"
         << " 2. Lister les passagers\n"
         << " 3. Ajouter un vol\n"
         << " 4. Lister les vols\n"
         << " 5. Creer une reservation\n"
         << " 6. Valider une reservation\n"
         << " 7. Payer une reservation\n"
         << " 8. Annuler une reservation\n"
         << " 9. Rembourser une reservation annulee\n"
         << "10. Lister les reservations\n"
         << "11. Lister les paiements\n"
         << " 0. Quitter\n";
}

int main() {
    Agence agence("Agence Cotonou Voyages");
    int choix = -1;

    while (choix != 0) {
        try {
            afficherMenu();
            choix = lireEntier("Votre choix : ");
            switch (choix) {
                case 1: {
                    string n = lireTexte("Nom : ");
                    string p = lireTexte("Prenom : ");
                    string t = lireTexte("Telephone : ");
                    string e = lireTexte("Email : ");
                    string pass = lireTexte("Numero de passeport : ");
                    int id = agence.ajouterPassager(n, p, t, e, pass);
                    cout << "Passager enregistre avec l'ID " << id << ".\n";
                    break;
                }
                case 2: agence.listerPassagers(); break;
                case 3: {
                    string num = lireTexte("Numero de vol : ");
                    string dep = lireTexte("Ville de depart : ");
                    string dest = lireTexte("Destination : ");
                    string date = lireTexte("Date (AAAA-MM-JJ) : ");
                    string heure = lireTexte("Heure (HH:MM) : ");
                    int cap = lireEntier("Capacite : ");
                    double prix = lireReel("Prix du billet (FCFA) : ");
                    int id = agence.ajouterVol(num, dep, dest, date, heure, cap, prix);
                    cout << "Vol cree avec l'ID " << id << ".\n";
                    break;
                }
                case 4: agence.listerVols(); break;
                case 5: {
                    int idP = lireEntier("ID du passager : ");
                    int idV = lireEntier("ID du vol : ");
                    int id = agence.reserver(idP, idV);
                    cout << "Reservation creee (ID " << id << ", en attente de validation).\n";
                    break;
                }
                case 6: {
                    agence.validerReservation(lireEntier("ID de la reservation : "));
                    cout << "Reservation validee.\n";
                    break;
                }
                case 7: {
                    int id = lireEntier("ID de la reservation : ");
                    string mode = lireTexte("Mode de paiement (Especes/Mobile Money/Carte) : ");
                    agence.payerReservation(id, mode);
                    cout << "Paiement enregistre.\n";
                    break;
                }
                case 8: {
                    agence.annulerReservation(lireEntier("ID de la reservation : "));
                    cout << "Reservation annulee (remboursement en attente si payee).\n";
                    break;
                }
                case 9: {
                    agence.rembourserReservation(lireEntier("ID de la reservation : "));
                    cout << "Remboursement effectue.\n";
                    break;
                }
                case 10: agence.listerReservations(); break;
                case 11: agence.listerPaiements(); break;
                case 0: cout << "Au revoir.\n"; break;
                default: cout << "Choix inconnu.\n";
            }
        } catch (const runtime_error& e) {
            string msg = e.what();
            if (msg == "Fin de saisie.") break;
            cout << "Erreur : " << msg << "\n";
        }
    }
    return 0;
}
