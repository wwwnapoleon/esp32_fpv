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
    attachPageHandlers(name);
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

function attachPageHandlers(name) {
  const reviewForm = document.getElementById('review-form');
  if (reviewForm) reviewForm.addEventListener('submit', handleReviewSubmit);
}

function handleReviewSubmit(e) {
  e.preventDefault();
  const name    = document.getElementById('review-name').value;
  const message = document.getElementById('review-message').value;
  const status  = document.getElementById('review-status');

  console.log('Отзыв:', { name, message });
  status.textContent = '✅ Спасибо, ' + name + '! Отзыв отправлен.';
  status.className = 'review-status success';
  e.target.reset();

  setTimeout(() => {
    status.textContent = '';
    status.className = 'review-status';
  }, 5000);
}

document.querySelectorAll('.menu-item[data-page]').forEach(btn => {
  btn.addEventListener('click', () => {
    const page = btn.getAttribute('data-page');
    loadPage(page);

    document.querySelectorAll('.menu-item').forEach(m => m.classList.remove('active'));
    btn.classList.add('active');

    if (window.innerWidth < 1024) closeMenu();
  });
});

document.addEventListener('DOMContentLoaded', () => {
  loadPage('news');
});
