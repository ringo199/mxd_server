const data = [];

const maxlv = 10;

for (let i = 1; i <= maxlv; ++i) {
    data.push({
        lv: i,
        hp: 100 + 20 * i,
        mp: 95 + 5 * i,
        exp: 50 * i,
        atk: 15 * i,
        def: 12 * i
    })
}

JSON.stringify(data);