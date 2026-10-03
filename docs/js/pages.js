// ============================================================
//  Динамическая загрузка страниц
// ============================================================

const contentArea = document.getElementById('content-area');
const pageTitle   = document.getElementById('page-title');

const PAGE_TITLES = {
  'directions': '🎯 Направления',
  'actions':    '♻️ Акции',
  'zhiza':      '📰 ЖИЗА',
  'news':       '📰 Новости проекта'
};

async function loadPage(name) {
  if (!name) name = 'news';

  if (PAGE_TITLES[name]) pageTitle.textContent = PAGE_TITLES[name];

  contentArea.innerHTML = '<div class="loading">Загрузка...</div>';

  try {
    const res = await fetch('content/' + name + '.html');
    if (!res.ok) throw new Error('HTTP ' + res.status);
    const html = await res.text();
    contentArea.innerHTML = html;
    window.scrollTo({ top: 0, behavior: 'smooth' });

    // Применяем анимации и счётчики после загрузки
    setTimeout(function() {
      applyScrollAnimations();
      initCounters();
      attachPageHandlers(name);
    }, 100);

  } catch (err) {
    console.error('Ошибка загрузки:', err);
    contentArea.innerHTML = `
      <div class="error-page">
        <h2>⚠️ Не удалось загрузить раздел</h2>
        <p>${name}.html — ${err.message}</p>
        <button onclick="loadPage('news')" class="btn">← На главную</button>
      </div>
    `;
  }
}

// ============================================================
//  Обработчики страниц
// ============================================================

function attachPageHandlers(name) {
  // Форма отзыва — только на странице ЖИЗЫ
  if (name !== 'zhiza') return;

  if (typeof getUser !== 'function') {
    console.warn('getUser не определена — auth.js не подключён');
    return;
  }

  const user = getUser();
  const formBlock   = document.getElementById('review-form-block');
  const lockedBlock = document.getElementById('review-locked');
  const nameInput   = document.getElementById('review-name');
  const reviewForm  = document.getElementById('review-form');

  if (!formBlock || !lockedBlock) return;

  if (user) {
    formBlock.style.display = 'block';
    lockedBlock.style.display = 'none';

    if (nameInput && user.fio) {
      nameInput.value = user.fio;
    }

    if (reviewForm && typeof handleReviewSubmit === 'function') {
      reviewForm.addEventListener('submit', handleReviewSubmit);
    }
  } else {
    formBlock.style.display = 'none';
    lockedBlock.style.display = 'block';
  }
}

// ============================================================
//  Отправка отзыва
// ============================================================

async function handleReviewSubmit(e) {
  e.preventDefault();

  const user = getUser();
  const message = document.getElementById('review-message').value.trim();
  const status  = document.getElementById('review-status');

  if (!user) {
    status.textContent = '🔒 Войдите, чтобы отправить отзыв';
    status.className = 'review-status error';
    return;
  }

  if (!message) {
    status.textContent = '⚠️ Введите сообщение';
    status.className = 'review-status error';
    return;
  }

  const name = user.fio || 'Аноним';

  if (SB) {
    status.textContent = '⏳ Отправка...';
    status.className = 'review-status';

    try {
      const { error } = await SB
        .from('reviews')
        .insert([{ name: name, message: message }]);

      if (error) throw error;

      status.textContent = '✅ Спасибо, ' + name + '! Отзыв отправлен на модерацию.';
      status.className = 'review-status success';

      document.getElementById('review-message').value = '';

      setTimeout(function() {
        status.textContent = '';
        status.className = 'review-status';
      }, 5000);

    } catch (err) {
      console.error('Supabase error:', err);
      status.textContent = '❌ Ошибка: ' + err.message;
      status.className = 'review-status error';
    }
    return;
  }

  // Демо-режим
  console.log('Отзыв (демо):', { name, message });
  status.textContent = '✅ Спасибо, ' + name + '! Отзыв отправлен (демо).';
  status.className = 'review-status success';
  document.getElementById('review-message').value = '';

  setTimeout(function() {
    status.textContent = '';
    status.className = 'review-status';
  }, 5000);
}

// ============================================================
//  Меню навигации
// ============================================================

document.querySelectorAll('.menu-item[data-page]').forEach(btn => {
  btn.addEventListener('click', () => {
    const page = btn.getAttribute('data-page');
    loadPage(page);

    document.querySelectorAll('.menu-item').forEach(m => m.classList.remove('active'));
    btn.classList.add('active');

    if (window.innerWidth < 1024 && typeof closeMenu === 'function') closeMenu();
  });
});

// ============================================================
//  Анимации при скролле
// ============================================================

const scrollObserver = new IntersectionObserver((entries) => {
  entries.forEach((entry, index) => {
    if (entry.isIntersecting) {
      // Задержка — максимум 4 элемента (320ms)
      const delay = Math.min(index, 4) * 80;
      setTimeout(() => {
        entry.target.classList.add('visible');
      }, delay);
      scrollObserver.unobserve(entry.target);
    }
  });
}, {
  threshold: 0,                      // ← было 0.1 — срабатывает раньше
  rootMargin: '0px 0px -20px 0px'    // ← было -40px
});

function applyScrollAnimations() {
  const selectors = [
    '.news-card',
    '.team-card',
    '.stat-card',
    '.how-step',
    '.tech-card',
    '.direction-card',
    '.action-card',
    '.blog-post',
    '.hero',
    '.join-section',
    '.cta-section'
  ];

  selectors.forEach(sel => {
    document.querySelectorAll(sel).forEach(el => {
      if (!el.classList.contains('animate-on-scroll')) {
        el.classList.add('animate-on-scroll');
        scrollObserver.observe(el);
      }
    });
  });
}

// ============================================================
//  Анимация счётчика статистики
// ============================================================

function animateCounter(el) {
  const target = parseInt(el.getAttribute('data-target'), 10);
  if (isNaN(target)) return;

  const suffix = el.getAttribute('data-suffix') || '';
  const duration = 2500;
  const startTime = performance.now();

  function update(now) {
    const elapsed = now - startTime;
    const progress = Math.min(elapsed / duration, 1);
    const eased = 1 - Math.pow(1 - progress, 4);
    const current = Math.round(target * eased);

    el.textContent = current + suffix;

    if (progress < 1) {
      requestAnimationFrame(update);
    } else {
      el.textContent = target + suffix;
    }
  }

  requestAnimationFrame(update);
}

const counterObserver = new IntersectionObserver((entries) => {
  entries.forEach(entry => {
    if (entry.isIntersecting) {
      animateCounter(entry.target);
      counterObserver.unobserve(entry.target);
    }
  });
}, {
  threshold: 0.3
});

function initCounters() {
  document.querySelectorAll('.stat-value[data-target], .stat-number[data-target]').forEach(el => {
    el.textContent = '0' + (el.getAttribute('data-suffix') || '');
    counterObserver.observe(el);
  });
}

// ============================================================
//  Загрузка страницы
// ============================================================

document.addEventListener('DOMContentLoaded', () => {
  loadPage('news');
});
