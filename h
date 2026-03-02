import React, { useState } from "react";
import { Card, CardContent } from "@/components/ui/card";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { motion } from "framer-motion";

function generateCode() {
  return Math.random().toString(36).substring(2, 10).toUpperCase();
}

export default function App() {
  const adminAccount = {
    id: 1,
    email: "nathangouvernet@gmail.com",
    password: "nathan090713",
    role: "admin",
    cards: [],
  };

  const [users, setUsers] = useState([adminAccount]);
  const [currentUser, setCurrentUser] = useState(null);
  const [selectedUserEmail, setSelectedUserEmail] = useState("");
  const [isLogin, setIsLogin] = useState(true);
  const [authData, setAuthData] = useState({ email: "", password: "" });
  const [page, setPage] = useState("cards");
  const [cashAmount, setCashAmount] = useState(0);
  const [transferEmail, setTransferEmail] = useState("");
  const [transferAmount, setTransferAmount] = useState(0);

  const handleAuth = () => {
    if (!authData.email || !authData.password) {
      alert("Remplis tous les champs");
      return;
    }

    if (isLogin) {
      const user = users.find(
        (u) => u.email === authData.email && u.password === authData.password
      );
      if (!user) {
        alert("Identifiants incorrects");
        return;
      }
      setCurrentUser(user);
    } else {
      if (users.find((u) => u.email === authData.email)) {
        alert("Utilisateur déjà existant");
        return;
      }
      const newUser = {
        id: Date.now(),
        email: authData.email,
        password: authData.password,
        role: "user",
        cards: [],
      };
      setUsers([...users, newUser]);
      setCurrentUser(newUser);
    }
  };

  const logout = () => {
    setCurrentUser(null);
    setAuthData({ email: "", password: "" });
  };

  const getTargetUser = () => {
    if (!currentUser) return null;
    if (currentUser.role === "admin") {
      return users.find((u) => u.email === selectedUserEmail) || null;
    }
    return currentUser;
  };

  const createCard = () => {
    const target = getTargetUser();
    if (!target) return alert("Sélectionne un utilisateur");

    const newCard = {
      id: Date.now(),
      code: generateCode(),
      balance: 0,
    };

    const updatedUser = {
      ...target,
      cards: [...target.cards, newCard],
    };

    setUsers(users.map((u) => (u.id === target.id ? updatedUser : u)));

    if (currentUser.role !== "admin") {
      setCurrentUser(updatedUser);
    }
  };

  const updateBalance = (code, amount) => {
    if (!currentUser || currentUser.role !== "admin") {
      alert("Seul l'admin peut modifier les soldes");
      return;
    }

    const target = getTargetUser();
    if (!target) return;

    const updatedCards = target.cards.map((card) =>
      card.code === code
        ? { ...card, balance: card.balance + amount }
        : card
    );

    const updatedUser = { ...target, cards: updatedCards };

    setUsers(users.map((u) => (u.id === target.id ? updatedUser : u)));
  };

  const transferMoney = () => {
    if (!currentUser) return;
    if (!transferEmail || transferAmount <= 0) {
      alert("Données de transfert invalides");
      return;
    }

    const receiver = users.find((u) => u.email === transferEmail);
    if (!receiver) {
      alert("Utilisateur destinataire introuvable");
      return;
    }

    if (!currentUser.cards || currentUser.cards.length === 0) {
      alert("Tu n'as pas de carte");
      return;
    }

    const senderCard = currentUser.cards[0];
    if (senderCard.balance < transferAmount) {
      alert("Solde insuffisant");
      return;
    }

    const updatedSenderCards = currentUser.cards.map((card, index) =>
      index === 0
        ? { ...card, balance: card.balance - transferAmount }
        : card
    );

    const updatedSender = { ...currentUser, cards: updatedSenderCards };

    let receiverCards = receiver.cards.length
      ? receiver.cards
      : [
          {
            id: Date.now(),
            code: generateCode(),
            balance: 0,
          },
        ];

    receiverCards = receiverCards.map((card, index) =>
      index === 0
        ? { ...card, balance: card.balance + transferAmount }
        : card
    );

    const updatedReceiver = { ...receiver, cards: receiverCards };

    setUsers(
      users.map((u) => {
        if (u.id === updatedSender.id) return updatedSender;
        if (u.id === updatedReceiver.id) return updatedReceiver;
        return u;
      })
    );

    setCurrentUser(updatedSender);
    alert("Transfert effectué !");
  };

  return (
    <div className="min-h-screen bg-gray-100 p-6">
      {!currentUser && (
        <Card className="max-w-md mx-auto p-6 rounded-2xl shadow-xl">
          <CardContent className="space-y-4">
            <h2 className="text-2xl font-bold text-center">
              {isLogin ? "Connexion" : "Inscription"}
            </h2>
            <Input
              placeholder="Email"
              value={authData.email}
              onChange={(e) =>
                setAuthData({ ...authData, email: e.target.value })
              }
            />
            <Input
              type="password"
              placeholder="Mot de passe"
              value={authData.password}
              onChange={(e) =>
                setAuthData({ ...authData, password: e.target.value })
              }
            />
            <Button onClick={handleAuth} className="w-full">
              {isLogin ? "Se connecter" : "S'inscrire"}
            </Button>
            <p
              className="text-center text-sm cursor-pointer text-blue-500"
              onClick={() => setIsLogin(!isLogin)}
            >
              {isLogin ? "Créer un compte" : "Déjà un compte ?"}
            </p>
          </CardContent>
        </Card>
      )}

      {currentUser && (
        <div>
          <div className="flex justify-between mb-6">
            <span className="font-bold">
              {currentUser.role === "admin" ? "ADMIN" : "Utilisateur"}
            </span>
            <Button onClick={logout}>Déconnexion</Button>
          </div>

          {currentUser.role === "admin" && (
            <div className="mb-4">
              <select
                className="p-2 rounded"
                onChange={(e) => setSelectedUserEmail(e.target.value)}
              >
                <option value="">Sélectionner un utilisateur</option>
                {users
                  .filter((u) => u.role !== "admin")
                  .map((u) => (
                    <option key={u.id} value={u.email}>
                      {u.email}
                    </option>
                  ))}
              </select>
            </div>
          )}

          <div className="mb-6 p-4 bg-white rounded-2xl shadow">
            <h3 className="font-bold mb-2">Envoyer de l'argent</h3>
            <Input
              placeholder="Email destinataire"
              value={transferEmail}
              onChange={(e) => setTransferEmail(e.target.value)}
            />
            <Input
              type="number"
              placeholder="Montant"
              className="mt-2"
              onChange={(e) => setTransferAmount(Number(e.target.value))}
            />
            <Button className="mt-2 w-full" onClick={transferMoney}>
              Envoyer
            </Button>
          </div>

          <Button onClick={createCard}>Créer une carte</Button>

          {(currentUser.role === "admin"
            ? users.find((u) => u.email === selectedUserEmail)?.cards || []
            : currentUser.cards
          ).map((card) => (
            <Card key={card.id} className="rounded-2xl shadow-lg mt-4">
              <CardContent className="p-6 space-y-3">
                <p><strong>Code:</strong> {card.code}</p>
                <p><strong>Solde:</strong> {card.balance} €</p>

                {currentUser.role === "admin" && (
                  <div className="flex gap-2">
                    <Input
                      type="number"
                      placeholder="Montant"
                      onChange={(e) =>
                        setCashAmount(Number(e.target.value))
                      }
                    />
                    <Button onClick={() => updateBalance(card.code, Number(cashAmount))}>
                      Ajouter
                    </Button>
                    <Button onClick={() => updateBalance(card.code, -Number(cashAmount))}>
                      Retirer
                    </Button>
                  </div>
                )}
              </CardContent>
            </Card>
          ))}
        </div>
      )}
    </div>
  );
}
