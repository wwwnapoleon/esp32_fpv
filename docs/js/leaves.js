// ============================================================
//  Листья-направления: реалистичные SVG + клик для раскрытия
//  Полная версия с авто-инициализацией
// ============================================================

// ---------- Формы листьев (реалистичные) ----------
function leafPath(variant) {
  const forms = {
    // 1. Виртуальное зрение — лист ивы (длинный, узкий)
    1: {
      outline: 'M50,4 C42,25 30,45 25,65 C20,82 35,96 50,98 C65,96 80,82 75,65 C70,45 58,25 50,4 Z',
      veins: [
        'M50,8 L50,95',
        'M50,25 Q42,30 36,35',
        'M50,25 Q58,30 64,35',
        'M50,45 Q40,50 33,55',
        'M50,45 Q60,50 67,55',
        'M50,65 Q42,68 37,72',
        'M50,65 Q58,68 63,72'
      ]
    },
    // 2. Аппаратная часть — дубовый лист (с зубчиками)
    2: {
      outline: 'M50,5 C38,10 32,18 35,25 C28,25 25,32 30,38 C24,42 24,50 30,55 C25,60 27,68 35,72 C32,80 38,90 50,96 C62,90 68,80 65,72 C73,68 75,60 70,55 C76,50 76,42 70,38 C75,32 72,25 65,25 C68,18 62,10 50,5 Z',
      veins: [
        'M50,10 L50,93',
        'M50,25 Q40,28 34,33',
        'M50,25 Q60,28 66,33',
        'M50,42 Q40,44 33,49',
        'M50,42 Q60,44 67,49',
        'M50,60 Q42,62 36,66',
        'M50,60 Q58,62 64,66'
      ]
    },
    // 3. Нейросети — кленовый лист
    3: {
      outline: 'M50,6 L55,20 L65,18 L60,30 L72,28 L65,38 L76,42 L64,48 L72,60 L60,58 L62,72 L52,66 L50,80 L48,66 L38,72 L40,58 L28,60 L36,48 L24,42 L35,38 L28,28 L40,30 L35,18 L45,20 Z',
      veins: [
        'M50,10 L50,76',
        'M50,30 L35,20',
        'M50,30 L65,20',
        'M50,45 L30,45',
        'M50,45 L70,45',
        'M50,55 L35,65',
        'M50,55 L65,65'
      ]
    },
    // 4. ПО — берёзовый лист (сердцевидный)
    4: {
      outline: 'M50,5 C30,12 18,30 18,52 C18,75 32,92 50,96 C68,92 82,75 82,52 C82,30 70,12 50,5 Z',
      veins: [
        'M50,8 L50,93',
        'M50,25 L33,32',
        'M50,25 L67,32',
        'M50,42 L30,48',
        'M50,42 L70,48',
        'M50,58 L33,63',
        'M50,58 L67,63',
        'M50,72 L38,76',
        'M50,72 L62,76'
      ]
    },
    // 5. Экология — лист с каплей
    5: {
      outline: 'M50,5 C40,20 25,35 22,55 C20,78 32,94 50,96 C68,94 80,78 78,55 C75,35 60,20 50,5 Z',
      veins: [
        'M50,10 L50,93',
        'M50,28 Q40,32 34,38',
        'M50,28 Q60,32 66,38',
        'M50,48 Q38,52 32,58',
        'M50,48 Q62,52 68,58',
        'M50,68 Q42,71 37,74',
        'M50,68 Q58,71 63,74'
      ],
      extra: '<circle cx="50" cy="42" r="3.5" fill="rgba(255,255,255,0.45)"/>'
    },
    // 6. Образование — простой лист с ровными краями
    6: {
      outline: 'M50,5 C35,10 22,28 20,50 C20,72 32,92 50,96 C68,92 80,72 80,50 C78,28 65,10 50,5 Z',
      veins: [
        'M50,8 L50,94',
        'M50,22 Q42,26 36,30',
        'M50,22 Q58,26 64,30',
        'M50,40 Q38,44 32,50',
        'M50,40 Q62,44 68,50',
        'M50,58 Q40,62 35,66',
        'M50,58 Q60,62 65,66',
        'M50,74 Q44,76 40,79',
        'M50,74 Q56,76 60,79'
      ]
    }
  };
  return forms[variant] || forms[1];
}

// ---------- SVG-шаблон реалистичного листа ----------
function leafSvg(variant) {
  const form = leafPath(variant);

  const colors = {
    1: { from: '#6fdd8b', to: '#2ea043' },
    2: { from: '#4ac26b', to: '#1f7d2e' },
    3: { from: '#8ee59e', to: '#3fb950' },
    4: { from: '#56d364', to: '#2ea043' },
    5: { from: '#6fdd8b', to: '#238636' },
    6: { from: '#a4e8b4', to: '#4ac26b' }
  };
  const c = colors[variant] || colors[1];
  const gradId = 'leafGrad' + variant + '_' + Math.random().toString(36).slice(2, 7);

  const veinsSvg = form.veins.map(function(d, i) {
    return '<path d="' + d + '" fill="none" stroke="rgba(0,0,0,0.22)" stroke-width="' + (i === 0 ? 0.9 : 0.5) + '" stroke-linecap="round"/>';
  }).join('');

  return '' +
    '<svg viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg" preserveAspectRatio="xMidYMid meet">' +
      '<defs>' +
        '<radialGradient id="' + gradId + '" cx="40%" cy="35%" r="75%">' +
          '<stop offset="0%" stop-color="' + c.from + '"/>' +
          '<stop offset="100%" stop-color="' + c.to + '"/>' +
        '</radialGradient>' +
      '</defs>' +
      '<path d="' + form.outline + '" fill="url(#' + gradId + ')" stroke="rgba(0,0,0,0.12)" stroke-width="0.6" stroke-linejoin="round"/>' +
      veinsSvg +
      (form.extra || '') +
    '</svg>';
}

// ---------- Данные о направлениях ----------
var DIRECTIONS = {
  vision: {
    icon: '👁️',
    title: 'виртуальное зрение',
    variant: 1,
    content: '' +
      '<h2>👁️ Виртуальное зрение</h2>' +
      '<p>Ключевая часть робота — «глаза», которые позволяют ему видеть мир.</p>' +
      '<h4>Что уже есть:</h4>' +
      '<ul>' +
        '<li>FPV-камера на ESP32-S3</li>' +
        '<li>Разрешение до 1600×1200</li>' +
        '<li>Поворотная платформа pan/tilt на MG90S</li>' +
        '<li>Трансляция видео в браузер по Wi-Fi</li>' +
      '</ul>' +
      '<h4>Что планируем:</h4>' +
      '<ul>' +
        '<li>Собственная математическая модель нейросети</li>' +
        '<li>Распознавание бутылок: положение, размер, цвет</li>' +
        '<li>Сравнение последовательных кадров</li>' +
        '<li>Автонаведение камеры на цель</li>' +
      '</ul>'
  },
  hardware: {
    icon: '🔧',
    title: 'аппаратная часть',
    variant: 2,
    content: '' +
      '<h2>🔧 Аппаратная часть</h2>' +
      '<p>Механика и электроника робота-сборщика пластика.</p>' +
      '<h4>Что уже есть:</h4>' +
      '<ul>' +
        '<li>ESP32-S3 как вычислительный центр</li>' +
        '<li>Камера OV2640 + 2× серво MG90S</li>' +
        '<li>Прототип корпуса</li>' +
      '</ul>' +
      '<h4>Что планируем:</h4>' +
      '<ul>' +
        '<li>Манипулятор для захвата бутылок</li>' +
        '<li>Колёсная база с моторами</li>' +
        '<li>Датчики препятствий</li>' +
        '<li>Контейнер с датчиком заполнения</li>' +
      '</ul>'
  },
  neural: {
    icon: '🧠',
    title: 'нейросети',
    variant: 3,
    content: '' +
      '<h2>🧠 Нейросети и математика</h2>' +
      '<p>Разрабатываем свою математическую модель для распознавания пластика.</p>' +
      '<h4>Ключевые задачи:</h4>' +
      '<ul>' +
        '<li>Положение бутылки в кадре (x, y)</li>' +
        '<li>Оценка размера в см</li>' +
        '<li>Распознавание цвета</li>' +
        '<li>Сравнение с предыдущими кадрами</li>' +
      '</ul>' +
      '<h4>Подходы:</h4>' +
      '<ul>' +
        '<li>OpenCV — детекция контуров</li>' +
        '<li>Свёрточные нейросети (CNN)</li>' +
        '<li>Математические методы</li>' +
      '</ul>'
  },
  software: {
    icon: '💻',
    title: 'программное обеспечение',
    variant: 4,
    content: '' +
      '<h2>💻 Программное обеспечение</h2>' +
      '<p>От прошивки ESP32 до веб-интерфейса оператора.</p>' +
      '<h4>Что уже есть:</h4>' +
      '<ul>' +
        '<li>Прошивка ESP32-S3 (PlatformIO)</li>' +
        '<li>Веб-интерфейс FPV</li>' +
        '<li>Управление серво по Wi-Fi</li>' +
      '</ul>' +
      '<h4>Что планируем:</h4>' +
      '<ul>' +
        '<li>Мобильное приложение</li>' +
        '<li>Запись и анализ видео</li>' +
        '<li>Облачная база данных</li>' +
      '</ul>'
  },
  ecology: {
    icon: '♻️',
    title: 'экология',
    variant: 5,
    content: '' +
      '<h2>♻️ Экология и переработка</h2>' +
      '<p>Цель проекта — автоматизировать сбор пластика.</p>' +
      '<h4>Проблема:</h4>' +
      '<ul>' +
        '<li>Миллионы бутылок попадают в природу</li>' +
        '<li>Ручной сбор трудоёмок</li>' +
        '<li>Пластик разлагается сотни лет</li>' +
      '</ul>' +
      '<h4>Решение:</h4>' +
      '<ul>' +
        '<li>Робот находит и собирает бутылки</li>' +
        '<li>Сортировка по цвету и типу</li>' +
        '<li>Статистика для школы</li>' +
      '</ul>'
  },
  education: {
    icon: '🎓',
    title: 'образование',
    variant: 6,
    content: '' +
      '<h2>🎓 Образование</h2>' +
      '<p>Проект учит программированию, электронике, математике, экологии.</p>' +
      '<h4>Что делаем:</h4>' +
      '<ul>' +
        '<li>Уроки с использованием робота</li>' +
        '<li>Мастер-классы по ESP32</li>' +
        '<li>Экскурсии для младших классов</li>' +
      '</ul>' +
      '<h4>Планы:</h4>' +
      '<ul>' +
        '<li>Кружок «Робототехника и ИИ»</li>' +
        '<li>Онлайн-курс по компьютерному зрению</li>' +
      '</ul>'
  }
};

// ---------- Рендер сетки листьев ----------
function renderLeaves() {
  var grid = document.getElementById('leaves-grid');
  if (!grid) return;

  var html = '';
  var i = 0;
  for (var key in DIRECTIONS) {
    if (!DIRECTIONS.hasOwnProperty(key)) continue;
    i++;
    var data = DIRECTIONS[key];
    html += '' +
      '<div class="leaf leaf--' + i + '" data-leaf="' + key + '">' +
        leafSvg(data.variant) +
        '<div class="leaf-content">' +
          '<div class="leaf-icon">' + data.icon + '</div>' +
          '<div class="leaf-title">' + data.title + '</div>' +
        '</div>' +
      '</div>';
  }
  grid.innerHTML = html;

  // Обработчики кликов
  var leafEls = grid.querySelectorAll('.leaf');
  for (var j = 0; j < leafEls.length; j++) {
    (function(leaf) {
      leaf.addEventListener('click', function() {
        var k = leaf.getAttribute('data-leaf');
        openLeaf(k);
      });
    })(leafEls[j]);
  }
}

// ---------- Открыть карточку листа ----------
function openLeaf(key) {
  var data = DIRECTIONS[key];
  if (!data) return;

  var modal = document.getElementById('leaf-expanded');
  if (!modal) {
    modal = document.createElement('div');
    modal.id = 'leaf-expanded';
    modal.className = 'leaf-expanded';
    document.body.appendChild(modal);
  }

  modal.innerHTML = '' +
    '<div class="leaf-expanded-card">' +
      '<button class="close-leaf" aria-label="Закрыть">×</button>' +
      data.content +
    '</div>';

  modal.classList.add('open');
  document.body.style.overflow = 'hidden';

  var closeBtn = modal.querySelector('.close-leaf');
  if (closeBtn) closeBtn.addEventListener('click', closeLeaf);

  modal.addEventListener('click', function(e) {
    if (e.target === modal) closeLeaf();
  });

  var escHandler = function(e) {
    if (e.key === 'Escape') {
      closeLeaf();
      document.removeEventListener('keydown', escHandler);
    }
  };
  document.addEventListener('keydown', escHandler);
}

function closeLeaf() {
  var modal = document.getElementById('leaf-expanded');
  if (modal) modal.classList.remove('open');
  document.body.style.overflow = '';
}

// ---------- Фоновые плавающие листья ----------
function addFloatingLeaves() {
  if (document.querySelector('.floating-leaves')) return; // уже есть

  var container = document.createElement('div');
  container.className = 'floating-leaves';

  var symbols = ['🍃', '🌿', '🍃', '🌱', '🌿'];
  for (var i = 0; i < 5; i++) {
    var leaf = document.createElement('div');
    leaf.className = 'floating-leaf';
    leaf.textContent = symbols[i];
    container.appendChild(leaf);
  }
  document.body.appendChild(container);
}

// ---------- Экспорт ----------
window.initLeaves = function() {
  renderLeaves();
  addFloatingLeaves();
  if (typeof window.initScrollAnimations === 'function') {
    window.initScrollAnimations();
  }
};

// ---------- Автозапуск ----------
// 1. Если листья уже в DOM (страница Направления загружена напрямую)
document.addEventListener('DOMContentLoaded', function() {
  if (document.getElementById('leaves-grid')) {
    setTimeout(function() {
      window.initLeaves();
    }, 100);
  }
});

// 2. Если #leaves-grid появился позже (через pages.js)
//    Наблюдаем за DOM и рендерим, когда контейнер появился
(function() {
  var observer = new MutationObserver(function() {
    var grid = document.getElementById('leaves-grid');
    var hasLeaves = document.querySelector('.leaf');
    if (grid && !hasLeaves) {
      window.initLeaves();
    }
  });
  observer.observe(document.body, { childList: true, subtree: true });
})();
