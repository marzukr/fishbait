/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    './src/**/*.{js,jsx,ts,tsx}',
  ],
  theme: {
    maxWidth: {
      sm: '375px',
    },
    extend: {
      colors: {
        green: {
          550: '#13B95B',
        },
        discord: '#5865F2',
      },
      fontFamily: {
        sans: ['Readex Pro', 'sans-serif'],
      }
    },
  },
  plugins: [],
  corePlugins: {
    preflight: false,
  }
}
