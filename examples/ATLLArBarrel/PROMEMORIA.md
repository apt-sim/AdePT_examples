# Promemoria: push e uso del branch sulla macchina con GPU

## 1. Push del branch (da questa macchina)

```sh
cd /home/parallels/atllarbarrel
git push -u origin fix/independent-review-followups
```

Se richiesto, inserire lo username CERN e il Personal Access Token al posto della password.

## 2. Codice sulla macchina con GPU

### Solo il branch, senza merge (consigliato fino alle prove con AdePT)

Se il repository non è ancora clonato:

```sh
git clone https://gitlab.cern.ch/dellacqu/atllarbarrel.git
cd atllarbarrel
git checkout fix/independent-review-followups
```

Se il repository è già clonato:

```sh
cd atllarbarrel
git fetch origin
git checkout fix/independent-review-followups
```

### Con il merge in master

Aprire la merge request su GitLab dal link stampato da `git push`, fare il merge, poi sulla macchina con GPU:

```sh
cd atllarbarrel
git checkout master
git pull
```

Nota: il comando `/adept/SetEnergyLossFluctuation true` aggiunto ai macro AdePT non è ancora stato eseguito con AdePT. Conviene fare il merge dopo le prove.
