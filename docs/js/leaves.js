// ============================================================
//  Листья-направления: SVG + клик для раскрытия
// ============================================================

// SVG-шаблон листа (универсальный)
function leafSvg(variant) {
  // Разные формы листьев для разнообразия
  const paths = {
    1: 'M50,5 C25,15 10,40 15,70 C18,90 35,100 50,95 C65,100 82,90 85,70 C90,40 75,15 50,5 Z M50,15 L50,90',
    2: 'M50,5 C20,20 15,55 25,80 C35,95 50,100 50,100 C50,100 65,95 75,80 C85,55 80,20 50,5 Z',
    3: 'M50,10 C30,10 15,30 15,55 C15,80 30,95 50,95 C70,95 85,80 85,55 C85,30 70,10 50,10 Z',
    4: 'M50,5 C40,25 20,35 15,55 C10,75 25,95 50,98 C75,95 90,75 85,55 C80,35 60,25 50,5 Z',
    5: 'M50,5 C30,15 12,35 15,60 C18,85 38,98 50,95 C62,98 82,85 85,60 C88,35 70,15 50,5 Z',
    6: 'M50,8 C25,18 10,45 18,72 C25,92 42,100 50,98 C58,100 75,92 82,72 C90,45 75,18 50,8 Z'
  };
  const path = paths[variant] || paths[1];

  return `
    <svg viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg" preserveAspectRatio="xMidYMid meet">
      <path d="${path}" stroke="rgba(0,0,0,0.08)" stroke-width="0.5" stroke-linejoin="round"/>
      <line x1="50" y1="15" x2="50" y2="90" stroke="rgba(0,0,0,0.15)" stroke-width="0.8" stroke-dasharray="2,2"/>
    </svg>
  `;
}

// Данные о направлениях
const DIRECTIONS = {
  vision: {
    icon: '👁️',
    title: 'виртуальное зрение',
    variant: 1,
    content: `
      <h2>👁️ Виртуальное зрение</h2>
      <p>Ключевая часть робота — «глаза», которые позволяют ему видеть мир.</p>
      <h4>Что уже есть:</h4>
      <ul>
        <li>FPV-камера на ESP32-S3</li>
        <li>Разрешение до 1600×1200</li>
        <li>Поворотная платформа pan/tilt на MG90S</li>
        <li>Трансляция видео в браузер по Wi-Fi</li>
      </ul>
      <h4>Что планируем:</h4>
      <ul>
        <li>Собственная математическая модель нейросети</li>
        <li>Распознавание бутылок: положение, размер, цвет</li>
        <li>Сравнение последовательных кадров</li>
        <li>Автонаведение камеры на цель</li>
      </ul>
    `
  },
  hardware: {
    icon: '🔧',
    title: 'аппаратная часть',
    variant: 2,
    content: `
      <h2>🔧 Аппаратная часть</h2>
      <p>Механика и электроника робота-сборщика пластика.</p>
      <h4>Что уже есть:</h4>
      <ul>
        <li>ESP32-S3 как вычислительный центр</li>
        <li>Камера OV2640 + 2× серво MG90S</li>
        <li>Прототип корпуса</li>
      </ul>
      <h4>Что планируем:</h4>
      <ul>
        <li>Манипулятор для захвата бутылок</li>
        <li>Колёсная база с моторами</li>
        <li>Датчики препятствий</li>
        <li>Контейнер с датчиком заполнения</li>
      </ul>
    `
  },
  neural: {
    icon: '🧠',
    title: 'нейросети',
    variant: 3,
    content: `
      <h2>🧠 Нейросети и математика</h2>
      <p>Разрабатываем свою математическую модель для распознавания пластика.</p>
      <h4>Ключевые задачи:</h4>
      <ul>
        <li>Положение бутылки в кадре (x, y)</li>
        <li>Оценка размера в см</li>
        <li>Распознавание цвета</li>
        <li>Сравнение с предыдущими кадрами</li>
      </ul>
      <h4>Подходы:</h4>
      <ul>
        <li>OpenCV — детекция контуров</li>
        <li>Свёрточные нейросети (CNN)</li>
        <li>Математические методы</li>
      </ul>
    `
  },
  software: {
    icon: '💻',
    title: 'программное обеспечение',
    variant: 4,
    content: `
      <h2>💻 Программное обеспечение</h2>
      <p>От прошивки ESP32 до веб-интерфейса оператора.</p>
      <h4>Что уже есть:</h4>
      <ul>
        <li>Прошивка ESP32-S3 (PlatformIO)</li>
        <li>Веб-интерфейс FPV</li>
        <li>Управление серво по Wi-Fi</li>
      </ul>
      <h4>Что планируем:</h4>
      <ul>
        <li>Мобильное приложение</li>
        <li>Запись и анализ видео</li>
        <li>Облачная база данных</li>
      </ul>
    `
  },
  ecology: {
    icon: '♻️',
    title: 'экология',
    variant: 5,
    content: `
      <h2>♻️ Экология и переработка</h2>
      <p>Цель проекта — автоматизировать сбор пластика.</p>
      <h4>Проблема:</h4>
      <ul>
        <li>Миллионы бутылок попадают в природу</li>
        <li>Ручной сбор трудоёмок</li>
        <li>Пластик разлагается сотни лет</li>
      </ul>
      <h4>Решение:</h4>
      <ul>
        <li>Робот находит и собирает бутылки</li>
        <li>Сортировка по цвету и типу</li>
        <li>Статистика для школы</li>
      </ul>
    `
  },
  education: {
    icon: '🎓',
    title: 'образование',
    variant: 6,
    content: `
      <h2>🎓 Образование</h2>
      <p>Проект учит программированию, электронике, математике, экологии.</p>
      <h4>Что делаем:</h4>
      <ul>
        <li>Уроки с использованием робота</li>
        <li>Мастер-классы по ESP32</li>
        <li>Экскурсии для младших классов</li>
      </ul>
      <h4>Планы:</h4>
      <ul>
        <li>Кружок «Робототехника и ИИ»</li>
        <li>Онлайн-курс по компьютерному зрению</li>
      </ul>
    `
  }
};

// ---------- Рендер сетки листьев ----------
function renderLeaves() {
  const grid = document.getElementById('leaves-grid');
  if (!grid) return;

  let html = '';
  let i = 0;
  for (const [key, data] of Object.entries(DIRECTIONS)) {
    i++;
    html += `
      <div class="leaf leaf--${i}" data-leaf="${key}">
        ${leafSvg(data.variant)}
        <div class="leaf-content">
          <div class="leaf-icon">${data.icon}</div>
          <div class="leaf-title">${data.title}</div>
        </div>
      </div>
    `;
  }
  grid.innerHTML = html;

  // Обработчики кликов
  grid.querySelectorAll('.leaf').forEach(leaf => {
    leaf.addEventListener('click', () => {
      const key = leaf.getAttribute('data-leaf');
      openLeaf(key);
    });
  });
}

// ---------- Открыть карточку листа ----------
function openLeaf(key) {
  const data = DIRECTIONS[key];
  if (!data) return;

  let modal = document.getElementById('leaf-expanded');
  if (!modal) {
    modal = document.createElement('div');
    modal.id = 'leaf-expanded';
    modal.className = 'leaf-expanded';
    document.body.appendChild(modal);
  }

  modal.innerHTML = `
    <div class="leaf-expanded-card">
      <button class="close-leaf" aria-label="Закрыть">×</button>
      ${data.content}
    </div>
  `;

  modal.classList.add('open');
  document.body.style.overflow = 'hidden';

  // Закрытие
  modal.querySelector('.close-leaf').addEventListener('click', closeLeaf);
  modal.addEventListener('click', (e) => {
    if (e.target === modal) closeLeaf();
  });
  document.addEventListener('keydown', function esc(e) {
    if (e.key === 'Escape') {
      closeLeaf();
      document.removeEventListener('keydown', esc);
    }
  });
}

function closeLeaf() {
  const modal = document.getElementById('leaf-expanded');
  if (modal) modal.classList.remove('open');
  document.body.style.overflow = '';
}

// ---------- Фоновые плавающие листья ----------
function addFloatingLeaves() {
  const container = document.createElement('div');
  container.className = 'floating-leaves';
  for (let i = 0; i < 5; i++) {
    const leaf = document.createElement('div');
    leaf.className = 'floating-leaf';
    leaf.textContent = ['🍃', '🌿', '🍃', '🌱', '🌿'][i];
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
