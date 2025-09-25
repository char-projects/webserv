
let colorInterval;

function getRandomColor() {
    const letters = '0123456789ABCDEF';
    let color = '#';
    for (let i = 0; i < 6; i++) {
        color += letters[Math.floor(Math.random() * 16)];
    }
    return color;
}

function changeColorOnce() {
    const colorBox = document.getElementById('colorBox');
    const newColor = getRandomColor();
    colorBox.style.backgroundColor = newColor;
}

function startColorChange() {
    colorInterval = setInterval(changeColorOnce, 500);
}

function stopColorChange() {
    if (colorInterval) {
        clearInterval(colorInterval);
    }
}

function getRandomRGBColor() {
    const r = Math.floor(Math.random() * 256);
    const g = Math.floor(Math.random() * 256);
    const b = Math.floor(Math.random() * 256);
    return `rgb(${r}, ${g}, ${b})`;
}

function getRandomPredefinedColor() {
    const colors = [
        '#FF6B6B', '#4ECDC4', '#45B7D1', '#96CEB4', '#FFEAA7',
        '#DDA0DD', '#98D8C8', '#F7DC6F', '#BB8FCE', '#85C1E9',
        '#F8C471', '#82E0AA', '#F1948A', '#85C1E9', '#D7BDE2'
    ];
    return colors[Math.floor(Math.random() * colors.length)];
}

document.addEventListener('DOMContentLoaded', function() {
    changeColorOnce();

    document.getElementById('colorBox').addEventListener('click', changeColorOnce);
});
