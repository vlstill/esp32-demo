---
vim: spelllang=cs
---

- SOS electronics

- dioda $10 - 20 mA$, úbytek $1.1 - 1.3V$
- vysoké napětí IO nožiček je $VCC = 3.3V$
  - mód nožičky bude push-pull (vysoké/nízké)
- Ohnův zákon: $I = \frac{U}{R}$
- potřebujeme cca $120 \Omega$ rezistory

- vstup
  - <= 0.6V -> 0
  * >= 2.7V -> 1
  - mezi tím nedefinované

- tlačítko - spíná opakovaně, lze řešit kondendzátorem paralelně k tlačítku:
  4-50 nF
